#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define HTTP_PORT 8080
#define BUFFER_SIZE 4096
#define MAX_CLIENTS 10
#define THREAD_POOL_SIZE 4
#define TASK_QUEUE_SIZE 1024

typedef struct {
  int queue[TASK_QUEUE_SIZE];
  int head;
  int tail;
  int count;
  int stop;
  pthread_t workers[THREAD_POOL_SIZE];
  pthread_mutex_t mutex;
  pthread_cond_t not_empty;
  pthread_cond_t not_full;
} ThreadPool;

void handle_request(int client_fd, char *request);

static ThreadPool g_pool;

typedef struct {
  const char *ext;
  const char *content_type;
} MimeMap;

static const MimeMap kMimeTypes[] = {
    {".html", "text/html; charset=utf-8"},
    {".htm", "text/html; charset=utf-8"},
    {".txt", "text/plain; charset=utf-8"},
    {".css", "text/css; charset=utf-8"},
    {".js", "application/javascript; charset=utf-8"},
    {".json", "application/json; charset=utf-8"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".mp4", "video/mp4"},
    {".webm", "video/webm"},
};

const char *get_content_type(const char *filepath) {
  const char *dot = strrchr(filepath, '.');
  size_t i;

  if (dot == NULL) {
    return "application/octet-stream";
  }

  for (i = 0; i < sizeof(kMimeTypes) / sizeof(kMimeTypes[0]); i++) {
    if (strcasecmp(dot, kMimeTypes[i].ext) == 0) {
      return kMimeTypes[i].content_type;
    }
  }

  return "application/octet-stream";
}

int parse_range_header(const char *request, off_t file_size, off_t *start,
                       off_t *end) {
  const char *range = strstr(request, "\r\nRange: bytes=");
  char *parse_end = NULL;
  long long a;
  long long b;

  if (range == NULL) {
    return 0;
  }

  range += strlen("\r\nRange: bytes=");

  if (*range == '-') {
    a = strtoll(range + 1, &parse_end, 10);
    if (parse_end == range + 1 || a <= 0) {
      return -1;
    }
    if (a >= (long long)file_size) {
      *start = 0;
    } else {
      *start = file_size - (off_t)a;
    }
    *end = file_size - 1;
    return 1;
  }

  a = strtoll(range, &parse_end, 10);
  if (parse_end == range || a < 0 || *parse_end != '-') {
    return -1;
  }

  if (parse_end[1] == '\r') {
    *start = (off_t)a;
    *end = file_size - 1;
  } else {
    b = strtoll(parse_end + 1, &parse_end, 10);
    if (parse_end == NULL || b < a) {
      return -1;
    }
    *start = (off_t)a;
    *end = (off_t)b;
  }

  if (*start >= file_size || *start < 0) {
    return -1;
  }
  if (*end >= file_size) {
    *end = file_size - 1;
  }
  if (*end < *start) {
    return -1;
  }

  return 1;
}

int is_subpath(const char *base, const char *path) {
  size_t base_len = strlen(base);

  if (strncmp(base, path, base_len) != 0) {
    return 0;
  }

  if (path[base_len] == '\0' || path[base_len] == '/') {
    return 1;
  }

  return 0;
}

int hex_value(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  }
  if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  }
  if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  }
  return -1;
}

int decode_uri_path(const char *raw_uri, char *decoded, size_t decoded_size) {
  size_t i = 0;
  size_t j = 0;

  while (raw_uri[i] != '\0' && raw_uri[i] != '?' && raw_uri[i] != '#') {
    if (j + 1 >= decoded_size) {
      return -1;
    }

    if (raw_uri[i] == '%') {
      int hi;
      int lo;

      if (raw_uri[i + 1] == '\0' || raw_uri[i + 2] == '\0') {
        return -1;
      }

      hi = hex_value(raw_uri[i + 1]);
      lo = hex_value(raw_uri[i + 2]);
      if (hi < 0 || lo < 0) {
        return -1;
      }

      decoded[j++] = (char)((hi << 4) | lo);
      i += 3;
      continue;
    }

    decoded[j++] = raw_uri[i++];
  }

  decoded[j] = '\0';
  return 0;
}

int build_safe_file_path(const char *decoded_uri, char *filepath,
                         size_t filepath_size) {
  char base_real[PATH_MAX];
  char target[PATH_MAX];
  char parent_copy[PATH_MAX];
  char parent_real[PATH_MAX];
  char file_real[PATH_MAX];
  char *parent_dir;

  if (decoded_uri[0] != '/') {
    return 400;
  }

  if (strstr(decoded_uri, "..") != NULL) {
    return 403;
  }

  if (realpath("./files", base_real) == NULL) {
    return 500;
  }

  if (snprintf(target, sizeof(target), "%s%s", base_real, decoded_uri) >=
      (int)sizeof(target)) {
    return 400;
  }

  if (snprintf(parent_copy, sizeof(parent_copy), "%s", target) >=
      (int)sizeof(parent_copy)) {
    return 400;
  }

  parent_dir = dirname(parent_copy);
  if (parent_dir == NULL) {
    return 400;
  }

  if (realpath(parent_dir, parent_real) != NULL) {
    if (!is_subpath(base_real, parent_real)) {
      return 403;
    }
  }

  if (realpath(target, file_real) != NULL) {
    if (!is_subpath(base_real, file_real)) {
      return 403;
    }
  }

  if (snprintf(filepath, filepath_size, "%s", target) >= (int)filepath_size) {
    return 400;
  }

  return 0;
}

int send_all(int fd, const void *buf, size_t len) {
  const char *p = (const char *)buf;
  size_t sent = 0;

  while (sent < len) {
    ssize_t n = send(fd, p + sent, len - sent, 0);
    if (n > 0) {
      sent += (size_t)n;
      continue;
    }

    if (n < 0 && (errno == EINTR)) {
      continue;
    }
    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      usleep(1000);
      continue;
    }
    return -1;
  }

  return 0;
}

/*
 * 发送 HTTP 响应
 */
void send_response_ex(int client_fd, const char *status, const char *content_type,
                      const char *extra_headers, const char *body, int body_len,
                      int send_body) {
  char header[BUFFER_SIZE];
  int header_len;

  /* 构建 HTTP 响应头 */
  if (extra_headers && extra_headers[0] != '\0') {
    header_len = snprintf(header, BUFFER_SIZE,
                          "HTTP/1.1 %s\r\n"
                          "Content-Type: %s\r\n"
                          "Content-Length: %d\r\n"
                          "%s"
                          "Connection: close\r\n"
                          "\r\n",
                          status, content_type, body_len, extra_headers);
  } else {
    header_len = snprintf(header, BUFFER_SIZE,
                          "HTTP/1.1 %s\r\n"
                          "Content-Type: %s\r\n"
                          "Content-Length: %d\r\n"
                          "Connection: close\r\n"
                          "\r\n",
                          status, content_type, body_len);
  }

  /* 发送响应头 */
  if (header_len <= 0) {
    return;
  }

  if (send_all(client_fd, header, (size_t)header_len) < 0) {
    perror("send header failed");
    return;
  }

  /* 发送响应体 */
  if (send_body && body && body_len > 0) {
    if (send_all(client_fd, body, (size_t)body_len) < 0) {
      perror("send body failed");
    }
  }
}

void send_response(int client_fd, const char *status, const char *content_type,
                   const char *body, int body_len) {
  send_response_ex(client_fd, status, content_type, NULL, body, body_len, 1);
}

void send_response_head(int client_fd, const char *status,
                        const char *content_type, int body_len,
                        const char *extra_headers) {
  send_response_ex(client_fd, status, content_type, extra_headers, NULL, body_len,
                   0);
}

/*
 * 发送文件
 */
void send_file(int client_fd, const char *filepath, int send_body,
               int enable_range, const char *request) {
  int file_fd;
  char buffer[BUFFER_SIZE];
  struct stat file_stat;
  char header[BUFFER_SIZE];
  int header_len;
  int read_bytes;
  const char *content_type;
  off_t range_start = 0;
  off_t range_end = 0;
  off_t content_len = 0;
  int range_result = 0;
  int is_partial = 0;

  /* 打开文件 */
  file_fd = open(filepath, O_RDONLY);
  if (file_fd < 0) {
    /* 文件不存在，返回 404 */
    send_response(client_fd, "404 Not Found", "text/html; charset=utf-8",
                  "<h1>404 Not Found</h1>", strlen("<h1>404 Not Found</h1>"));
    return;
  }

  /* 获取文件大小 */
  if (fstat(file_fd, &file_stat) < 0) {
    close(file_fd);
    send_response(client_fd, "500 Internal Server Error",
                  "text/html; charset=utf-8",
                  "<h1>500 Internal Server Error</h1>",
                  strlen("<h1>500 Internal Server Error</h1>"));
    return;
  }

  content_type = get_content_type(filepath);

  if (enable_range && request != NULL) {
    range_result = parse_range_header(request, file_stat.st_size, &range_start,
                                      &range_end);
    if (range_result < 0) {
      char range_hdr[128];
      int hdr_len = snprintf(range_hdr, sizeof(range_hdr),
                             "Content-Range: bytes */%ld\r\n",
                             (long)file_stat.st_size);
      if (hdr_len < 0) {
        hdr_len = 0;
      }
      send_response_head(client_fd, "416 Range Not Satisfiable", content_type, 0,
                         range_hdr);
      close(file_fd);
      return;
    }
  }

  if (range_result == 1) {
    is_partial = 1;
    content_len = range_end - range_start + 1;
  } else {
    range_start = 0;
    range_end = file_stat.st_size - 1;
    content_len = file_stat.st_size;
  }

  /* 发送响应头 */
  if (is_partial) {
    header_len = snprintf(header, BUFFER_SIZE,
                          "HTTP/1.1 206 Partial Content\r\n"
                          "Content-Type: %s\r\n"
                          "Content-Length: %ld\r\n"
                          "Content-Range: bytes %ld-%ld/%ld\r\n"
                          "Accept-Ranges: bytes\r\n"
                          "Connection: close\r\n"
                          "\r\n",
                          content_type, (long)content_len, (long)range_start,
                          (long)range_end, (long)file_stat.st_size);
  } else {
    header_len = snprintf(header, BUFFER_SIZE,
                          "HTTP/1.1 200 OK\r\n"
                          "Content-Type: %s\r\n"
                          "Content-Length: %ld\r\n"
                          "Accept-Ranges: bytes\r\n"
                          "Connection: close\r\n"
                          "\r\n",
                          content_type, (long)content_len);
  }

  if (header_len <= 0 || send_all(client_fd, header, (size_t)header_len) < 0) {
    perror("send file header failed");
    close(file_fd);
    return;
  }

  if (!send_body) {
    close(file_fd);
    return;
  }

  if (lseek(file_fd, range_start, SEEK_SET) < 0) {
    perror("lseek file failed");
    close(file_fd);
    return;
  }

  /* 发送文件内容 */
  while (content_len > 0 &&
         (read_bytes = read(file_fd, buffer,
                            (content_len > BUFFER_SIZE) ? BUFFER_SIZE
                                                        : (size_t)content_len)) >
             0) {
    if (send_all(client_fd, buffer, (size_t)read_bytes) < 0) {
      perror("send file chunk failed");
      break;
    }
    content_len -= (off_t)read_bytes;
  }

  close(file_fd);
}

/*
 * 解析 HTTP 请求
 */
void handle_request(int client_fd, char *request) {
  int is_head;
  char method[16], uri[256], version[16];
  char decoded_uri[256];
  char filepath[512];
  int parsed;
  int path_status;

  /* 解析请求行 */
  parsed = sscanf(request, "%15s %255s %15s", method, uri, version);
  if (parsed != 3) {
    send_response(client_fd, "400 Bad Request", "text/plain; charset=utf-8",
                  "Malformed request line\n",
                  strlen("Malformed request line\n"));
    return;
  }

  if (decode_uri_path(uri, decoded_uri, sizeof(decoded_uri)) < 0) {
    send_response(client_fd, "400 Bad Request", "text/plain; charset=utf-8",
                  "Invalid URI encoding\n",
                  strlen("Invalid URI encoding\n"));
    return;
  }

  printf("Request: %s %s %s\n", method, decoded_uri, version);

  is_head = (strcmp(method, "HEAD") == 0);

  /* 处理 GET/HEAD 方法 */
  if (strcmp(method, "GET") != 0 && !is_head) {
    send_response(client_fd, "405 Method Not Allowed",
                  "text/html; charset=utf-8", "<h1>405 Method Not Allowed</h1>",
                  strlen("<h1>405 Method Not Allowed</h1>"));
    return;
  }

  /* 处理根路径 */
  if (strcmp(decoded_uri, "/") == 0) {
    send_file(client_fd, "./files/html/default.html", !is_head, 0, request);
    return;
  }

  if (strcmp(decoded_uri, "/hello") == 0) {
    const char *text =
        "Hello from C HTTP Server!\n"
        "Try: /image and /video\n";
    if (is_head) {
      send_response_head(client_fd, "200 OK", "text/plain; charset=utf-8",
                         strlen(text), NULL);
    } else {
      send_response(client_fd, "200 OK", "text/plain; charset=utf-8", text,
                    strlen(text));
    }
    return;
  }

  if (strcmp(decoded_uri, "/image") == 0) {
    send_file(client_fd, "./files/image/阿能.jpg", !is_head, 0, request);
    return;
  }

  if (strcmp(decoded_uri, "/video") == 0) {
    send_file(client_fd,
              "./files/video/video_写出这样的重定位算法可以找到工作..._0.mp4",
              !is_head, 1, request);
    return;
  }

  path_status = build_safe_file_path(decoded_uri, filepath, sizeof(filepath));
  if (path_status == 403) {
    send_response(client_fd, "403 Forbidden", "text/plain; charset=utf-8",
                  "Forbidden path\n", strlen("Forbidden path\n"));
    return;
  }
  if (path_status == 500) {
    send_response(client_fd, "500 Internal Server Error",
                  "text/plain; charset=utf-8", "Path resolution failed\n",
                  strlen("Path resolution failed\n"));
    return;
  }
  if (path_status != 0) {
    send_response(client_fd, "400 Bad Request", "text/plain; charset=utf-8",
                  "Invalid path\n", strlen("Invalid path\n"));
    return;
  }

  /* 发送文件 */
  send_file(client_fd, filepath, !is_head, 0, request);
}

int setnonblocking(int sockfd) {
  int flags;
  flags = fcntl(sockfd, F_GETFL, 0);
  if (flags < 0)
    return -1;
  if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
    return -1;
  }
  return 0;
}

void removefd(int epoll_fd, int fd) {
  if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0) {
    if (errno != EBADF && errno != ENOENT) {
      perror("epoll_ctl del fd failed!");
    }
  }
}

void close_client_fd(int fd) {
  if (fd >= 0) {
    close(fd);
  }
}

void process_client_connection(int fd) {
  char buffer[BUFFER_SIZE];
  int total = 0;

  memset(buffer, 0, sizeof(buffer));

  while (total < BUFFER_SIZE - 1) {
    int ret = recv(fd, buffer + total, BUFFER_SIZE - 1 - total, 0);

    if (ret > 0) {
      total += ret;
      buffer[total] = '\0';
      if (strstr(buffer, "\r\n\r\n") != NULL) {
        break;
      }
      continue;
    }

    if (ret == 0) {
      printf("Client=%d disconnected and closed\n", fd);
      close_client_fd(fd);
      return;
    }

    if (errno == EINTR) {
      continue;
    }

    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      break;
    }

    perror("recv fail");
    printf("Client=%d error, closed\n", fd);
    close_client_fd(fd);
    return;
  }

  if (total <= 0 || strstr(buffer, "\r\n\r\n") == NULL) {
    send_response(fd, "400 Bad Request", "text/plain; charset=utf-8",
                  "Incomplete HTTP request\n",
                  strlen("Incomplete HTTP request\n"));
    printf("Client=%d bad request, closed\n", fd);
    close_client_fd(fd);
    return;
  }

  printf("Received request from client fd=%d %s len=%d\n", fd, buffer, total);
  handle_request(fd, buffer);
  close_client_fd(fd);
  printf("Client=%d response sent and closed\n", fd);
}

int thread_pool_submit(ThreadPool *pool, int client_fd) {
  pthread_mutex_lock(&pool->mutex);
  while (!pool->stop && pool->count == TASK_QUEUE_SIZE) {
    pthread_cond_wait(&pool->not_full, &pool->mutex);
  }

  if (pool->stop) {
    pthread_mutex_unlock(&pool->mutex);
    return -1;
  }

  pool->queue[pool->tail] = client_fd;
  pool->tail = (pool->tail + 1) % TASK_QUEUE_SIZE;
  pool->count++;

  pthread_cond_signal(&pool->not_empty);
  pthread_mutex_unlock(&pool->mutex);
  return 0;
}

int thread_pool_take(ThreadPool *pool) {
  int client_fd;

  pthread_mutex_lock(&pool->mutex);
  while (!pool->stop && pool->count == 0) {
    pthread_cond_wait(&pool->not_empty, &pool->mutex);
  }

  if (pool->stop && pool->count == 0) {
    pthread_mutex_unlock(&pool->mutex);
    return -1;
  }

  client_fd = pool->queue[pool->head];
  pool->head = (pool->head + 1) % TASK_QUEUE_SIZE;
  pool->count--;

  pthread_cond_signal(&pool->not_full);
  pthread_mutex_unlock(&pool->mutex);
  return client_fd;
}

void *worker_routine(void *arg) {
  ThreadPool *pool = (ThreadPool *)arg;

  while (1) {
    int client_fd = thread_pool_take(pool);
    if (client_fd < 0) {
      break;
    }
    process_client_connection(client_fd);
  }

  return NULL;
}

int thread_pool_init(ThreadPool *pool) {
  int i;

  memset(pool, 0, sizeof(*pool));
  if (pthread_mutex_init(&pool->mutex, NULL) != 0) {
    return -1;
  }
  if (pthread_cond_init(&pool->not_empty, NULL) != 0) {
    pthread_mutex_destroy(&pool->mutex);
    return -1;
  }
  if (pthread_cond_init(&pool->not_full, NULL) != 0) {
    pthread_cond_destroy(&pool->not_empty);
    pthread_mutex_destroy(&pool->mutex);
    return -1;
  }

  for (i = 0; i < THREAD_POOL_SIZE; i++) {
    if (pthread_create(&pool->workers[i], NULL, worker_routine, pool) != 0) {
      pool->stop = 1;
      pthread_cond_broadcast(&pool->not_empty);
      pthread_cond_broadcast(&pool->not_full);
      while (i-- > 0) {
        pthread_join(pool->workers[i], NULL);
      }
      pthread_cond_destroy(&pool->not_full);
      pthread_cond_destroy(&pool->not_empty);
      pthread_mutex_destroy(&pool->mutex);
      return -1;
    }
  }

  return 0;
}

void thread_pool_destroy(ThreadPool *pool) {
  int i;

  pthread_mutex_lock(&pool->mutex);
  pool->stop = 1;
  pthread_cond_broadcast(&pool->not_empty);
  pthread_cond_broadcast(&pool->not_full);
  pthread_mutex_unlock(&pool->mutex);

  for (i = 0; i < THREAD_POOL_SIZE; i++) {
    pthread_join(pool->workers[i], NULL);
  }

  pthread_cond_destroy(&pool->not_full);
  pthread_cond_destroy(&pool->not_empty);
  pthread_mutex_destroy(&pool->mutex);
}

int main() {
  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len;
  int ret;

  int epoll_fd = epoll_create(MAX_CLIENTS);
  if (epoll_fd < 0) {
    fprintf(stderr, "Failed to create epoll file descriptor\n");
    return -1;
  }
  printf("Epoll fd: %d\n", epoll_fd);

  if (thread_pool_init(&g_pool) != 0) {
    fprintf(stderr, "Failed to initialize thread pool\n");
    close(epoll_fd);
    return -1;
  }
  printf("Thread pool initialized with %d workers\n", THREAD_POOL_SIZE);

  /* 创建 TCP 套接字 */

  server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_fd < 0) {
    perror("socket fail");
    close(epoll_fd);
    return -1;
  }

  /* 设置端口复用 */
  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  setnonblocking(server_fd);

  /* 绑定地址 */
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(HTTP_PORT);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  ret = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (ret < 0) {
    perror("bind fail");
    close(server_fd);
    close(epoll_fd);
    return -1;
  }

  printf("bind successfully!\n");

  /* 开始监听 */
  ret = listen(server_fd, 10);
  if (ret < 0) {
    perror("listen fail");
    close(server_fd);
    return -1;
  }

  struct epoll_event ev;
  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = server_fd;

  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) < 0) {
    perror("epoll_ctl add server_fd fail!");
    close(server_fd);
    close(epoll_fd);
    return -1;
  }

  printf("server_fd added to epoll_fd successfully!");

  struct epoll_event events[MAX_CLIENTS];

  printf("HTTP Server listening on port %d...\n", HTTP_PORT);
  printf("Open browser: http://127.0.0.1:%d\n", HTTP_PORT);

  while (1) {
    int nfds = epoll_wait(epoll_fd, events, MAX_CLIENTS, -1);
    if (nfds < 0) {
      perror("epoll_wait fail!");
      break;
    }

    for (int i = 0; i < nfds; i++) {
      int fd = events[i].data.fd;

      if (fd == server_fd) {
        while (1) {
          client_len = sizeof(client_addr);
          client_fd =
              accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
          if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
              /* ET + nonblocking: no more pending connections in this round */
              break;
            } else {
              perror("accept fail");
              break;
            }
          }

          printf("Client connected: fd=%d, IP=%s, Port=%d\n", client_fd,
                 inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

          if (setnonblocking(client_fd) < 0) {
            perror("setnonblocking client_fd fail");
            close(client_fd);
            continue;
          }

          ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
          ev.data.fd = client_fd;
          if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
            perror("epoll_ctl add client_fd fail!");
            close(client_fd);
            continue;
          }
        }
      } else {
        removefd(epoll_fd, fd);
        if (thread_pool_submit(&g_pool, fd) != 0) {
          close(fd);
        }
      }
    }
  }

  close(server_fd);
  close(epoll_fd);
  thread_pool_destroy(&g_pool);
  return 0;
}

//   /* 主循环 */
//   while (1) {
//     client_len = sizeof(client_addr);
//     client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
//     &client_len); if (client_fd < 0) {
//       perror("accept fail");
//       continue;
//     }

//     printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr),
//            ntohs(client_addr.sin_port));

//     /* 接收 HTTP 请求 */
//     memset(buffer, 0, BUFFER_SIZE);
//     ret = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
//     if (ret <= 0) {
//       close(client_fd);
//       continue;
//     }

//     printf("Request:\n%s\n", buffer);

//     /* 处理请求并发送响应 */
//     handle_request(client_fd, buffer);

//     /* 关闭客户端连接 */
//     close(client_fd);
//   }

//   close(server_fd);
//   return 0;
