#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define HTTP_PORT 8080
#define BUFFER_SIZE 4096
#define MAX_CLIENTS 10
/*
 * 发送 HTTP 响应
 */
void send_response(int client_fd, const char *status, const char *content_type,
                   const char *body, int body_len) {
  char header[BUFFER_SIZE];
  int header_len;

  /* 构建 HTTP 响应头 */
  header_len = snprintf(header, BUFFER_SIZE,
                        "HTTP/1.1 %s\r\n"
                        "Content-Type: %s\r\n"
                        "Content-Length: %d\r\n"
                        "Connection: close\r\n"
                        "\r\n",
                        status, content_type, body_len);

  /* 发送响应头 */
  send(client_fd, header, header_len, 0);

  /* 发送响应体 */
  if (body && body_len > 0) {
    send(client_fd, body, body_len, 0);
  }
}

/*
 * 发送 HTML 页面
 */
void send_html(int client_fd, const char *html) {
  send_response(client_fd, "200 OK", "text/html; charset=utf-8", html,
                strlen(html));
}

/*
 * 发送文件
 */
void send_file(int client_fd, const char *filepath) {
  int file_fd;
  char buffer[BUFFER_SIZE];
  struct stat file_stat;
  char header[BUFFER_SIZE];
  int header_len;
  int read_bytes;

  /* 打开文件 */
  file_fd = open(filepath, O_RDONLY);
  if (file_fd < 0) {
    /* 文件不存在，返回 404 */
    send_response(client_fd, "404 Not Found", "text/html; charset=utf-8",
                  "<h1>404 Not Found</h1>", strlen("<h1>404 Not Found</h1>"));
    return;
  }

  /* 获取文件大小 */
  fstat(file_fd, &file_stat);

  /* 根据文件类型确定 Content-Type */
  const char *content_type = "application/octet-stream";
  if (strstr(filepath, ".html") || strstr(filepath, ".htm")) {
    content_type = "text/html; charset=utf-8";
  } else if (strstr(filepath, ".jpg") || strstr(filepath, ".jpeg")) {
    content_type = "image/jpeg";
  } else if (strstr(filepath, ".png")) {
    content_type = "image/png";
  } else if (strstr(filepath, ".mp4")) {
    content_type = "video/mp4";
  } else if (strstr(filepath, ".txt")) {
    content_type = "text/plain";
  }

  /* 发送响应头 */
  header_len = snprintf(header, BUFFER_SIZE,
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: %s\r\n"
                        "Content-Length: %ld\r\n"
                        "Connection: close\r\n"
                        "\r\n",
                        content_type, (long)file_stat.st_size);

  send(client_fd, header, header_len, 0);

  /* 发送文件内容 */
  while ((read_bytes = read(file_fd, buffer, BUFFER_SIZE)) > 0) {
    send(client_fd, buffer, read_bytes, 0);
  }

  close(file_fd);
}

void method_handler(int client_fd, const char *method) {}

void root_path_handler(int client_fd, const char *uri) {
  /* 处理根路径 */
  if (strcmp(uri, "/") == 0) {
    /* 返回默认 HTML 页面 */
    const char *html = "<!DOCTYPE html>\n"
                       "<html>\n"
                       "<head>\n"
                       "    <meta charset=\"UTF-8\">\n"
                       "    <title>HTTP Server Demo</title>\n"
                       "</head>\n"
                       "<body>\n"
                       "    <h1>Welcome to HTTP Server</h1>\n"
                       "    <h2>功能选择：</h2>\n"
                       "    <ul>\n"
                       "        <li><a href=\"/hello\">获取文本信息</a></li>\n"
                       "        <li><a href=\"/test.jpg\">获取图片</a></li>\n"
                       "        <li><a href=\"/test.mp4\">获取视频</a></li>\n"
                       "    </ul>\n"
                       "</body>\n"
                       "</html>";
    send_html(client_fd, html);
  }
}

/*
 * 解析 HTTP 请求
 */
void handle_request(int client_fd, char *request) {
  char method[16], uri[256], version[16];
  char filepath[512];

  /* 解析请求行 */
  sscanf(request, "%s %s %s", method, uri, version);

  printf("Request: %s %s %s\n", method, uri, version);

  /* 只处理 GET 方法 */
  if (strcmp(method, "GET") != 0) {
    send_response(client_fd, "405 Method Not Allowed",
                  "text/html; charset=utf-8", "<h1>405 Method Not Allowed</h1>",
                  strlen("<h1>405 Method Not Allowed</h1>"));
    return;
  }

  /* 处理根路径 */
  if (strcmp(uri, "/") == 0) {
    /* 返回默认 HTML 页面 */
    const char *html = "<!DOCTYPE html>\n"
                       "<html>\n"
                       "<head>\n"
                       "    <meta charset=\"UTF-8\">\n"
                       "    <title>HTTP Server Demo</title>\n"
                       "</head>\n"
                       "<body>\n"
                       "    <h1>Welcome to HTTP Server</h1>\n"
                       "    <h2>功能选择：</h2>\n"
                       "    <ul>\n"
                       "        <li><a href=\"/hello\">获取文本信息</a></li>\n"
                       "        <li><a href=\"/test.jpg\">获取图片</a></li>\n"
                       "        <li><a href=\"/test.mp4\">获取视频</a></li>\n"
                       "    </ul>\n"
                       "</body>\n"
                       "</html>";
    send_html(client_fd, html);
    return;
  }

  /* 解析 URI，去除前面的 / */
  strcpy(filepath, ".");
  strcat(filepath, uri);

  /* 发送文件 */
  send_file(client_fd, filepath);
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

int main() {
  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len;
  char buffer[BUFFER_SIZE];
  int ret;

  int epoll_fd = epoll_create(MAX_CLIENTS);
  if (epoll_fd < 0) {
    fprintf(stderr, "Failed to create epoll file descriptor\n");
    return -1;
  }
  printf("Epoll fd: %d\n", epoll_fd);

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

          ev.events = EPOLLIN | EPOLLET;
          ev.data.fd = client_fd;
          if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
            perror("epoll_ctl add client_fd fail!");
            close(client_fd);
            continue;
          }
        }
      } else {
        while (1) {
          memset(buffer, 0, BUFFER_SIZE);

          ret = recv(fd, buffer, BUFFER_SIZE - 1, 0);

          if (ret < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
              break;
            }
            perror("recv fail");
            removefd(epoll_fd, fd);
            close(fd);
            printf("Client=%d error, closed\n", fd);
            break;
          } else if (ret == 0) {
            removefd(epoll_fd, fd);
            close(fd);
            printf("Client=%d disconnected and closed\n", fd);
            break;
          } else {
            printf("Received request from client fd=%d %s len=%d\n", fd, buffer,
                   ret);

            char send_buf[128];
            int len = snprintf(send_buf, sizeof(send_buf),
                               "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n");
            send(fd, send_buf, len, 0);
          }
        }
      }
    }
  }

  close(server_fd);
  close(epoll_fd);
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
