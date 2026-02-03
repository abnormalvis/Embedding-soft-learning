# select/poll/epoll：IO 多路复用概览

IO 多路复用解决的问题：**同时等待多个 FD 的可读/可写事件**，避免为每个 FD 开一个线程或在非阻塞下忙等。

## 1. 三者对比（直觉版）

- `select`：接口老、FD 数量受限（`FD_SETSIZE`），每次需要重置集合。
- `poll`：没有 `FD_SETSIZE` 这种硬限制，接口更直观，但每次仍要线性扫描。
- `epoll`（Linux 特有）：把关注的 FD 注册到内核，适合大量连接；支持水平/边缘触发。

## 2. poll 入门（推荐先学）

```c
#include <poll.h>

int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```

- `timeout`：毫秒；`-1` 永久等待；`0` 立即返回。
- `fds[i].events`：关心的事件（如 `POLLIN`）
- 返回后看 `fds[i].revents`

## 3. epoll 核心 API

```c
#include <sys/epoll.h>

int epoll_create1(int flags);
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
```

触发模式：

- 水平触发（LT，默认）：FD 只要仍可读/可写，事件会持续出现。
- 边缘触发（ET）：状态从“不可用”变为“可用”时通知一次；通常需要把 FD 设为非阻塞并“读到 EAGAIN”。

## 4. 常见坑

- 非阻塞 + ET：必须循环 `read` 到 `EAGAIN`，否则可能丢事件。
- 处理 `EINTR`：`poll/epoll_wait` 也可能被信号打断。

## 5. 练习

1. 用 `poll` 监听 stdin 和一个 pipe：同时支持输入与来自子进程的数据。
2. 用 `epoll` 写一个最小 echo server（可后续扩展 socket 章节）。
### 参考实现

**练习 1: 用 poll 监听 stdin 和 pipe**

```c
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // 子进程：定期写入 pipe
        close(pipefd[0]);  // 关闭读端
        
        for (int i = 0; i < 5; i++) {
            sleep(1);
            char msg[32];
            snprintf(msg, sizeof(msg), "Message %d from child\n", i);
            write(pipefd[1], msg, strlen(msg));
        }
        
        close(pipefd[1]);
        return 0;
    }

    // 父进程：用 poll 同时监听 stdin 和 pipe
    close(pipefd[1]);  // 关闭写端

    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    
    fds[1].fd = pipefd[0];
    fds[1].events = POLLIN;

    printf("Monitoring stdin and pipe from child (type 'quit' to exit):\n");

    char buf[256];
    int child_done = 0;

    while (!child_done) {
        int ret = poll(fds, 2, 1000);  // 1 秒超时

        if (ret < 0) {
            perror("poll");
            break;
        }

        // 检查 stdin
        if (fds[0].revents & POLLIN) {
            ssize_t n = read(STDIN_FILENO, buf, sizeof(buf) - 1);
            if (n > 0) {
                buf[n] = '\0';
                if (strcmp(buf, "quit\n") == 0) {
                    printf("User quit\n");
                    break;
                }
                printf("User input: %s", buf);
            }
        }

        // 检查 pipe
        if (fds[1].revents & POLLIN) {
            ssize_t n = read(pipefd[0], buf, sizeof(buf) - 1);
            if (n > 0) {
                buf[n] = '\0';
                printf("From child: %s", buf);
            } else if (n == 0) {
                printf("Child closed pipe\n");
                child_done = 1;
            }
        }

        // 检查错误
        if (fds[1].revents & POLLHUP) {
            printf("Pipe closed\n");
            child_done = 1;
        }
    }

    close(pipefd[0]);
    wait(NULL);

    return 0;
}
```

编译运行：
```bash
gcc -o poll_demo poll_demo.c
./poll_demo
# 输入文本或等待子进程的消息
```

**练习 2: 用 epoll 写简单 echo server**

```c
#include <unistd.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_EVENTS 64
#define BUFFER_SIZE 256

void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(void) {
    // 为了演示，使用 stdin/stdout（在实际应用中应使用 socket）
    // 这里创建一个简单的模拟：监听 stdin 和 pipe
    
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    set_nonblocking(STDIN_FILENO);
    set_nonblocking(pipefd[0]);

    // 创建 epoll 实例
    int epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1");
        return 1;
    }

    // 添加 stdin 到 epoll
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
        perror("epoll_ctl stdin");
        return 1;
    }

    // 添加 pipe 读端到 epoll
    ev.data.fd = pipefd[0];
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, pipefd[0], &ev) == -1) {
        perror("epoll_ctl pipe");
        return 1;
    }

    printf("Simple epoll echo server (type 'quit' to exit):\n");

    char buf[BUFFER_SIZE];
    int running = 1;

    while (running) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, 1000);
        
        if (nfds < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == STDIN_FILENO) {
                // stdin 数据到达
                ssize_t n = read(STDIN_FILENO, buf, sizeof(buf) - 1);
                if (n > 0) {
                    buf[n] = '\0';
                    if (strcmp(buf, "quit\n") == 0) {
                        printf("Exiting...\n");
                        running = 0;
                        break;
                    }
                    // echo：写回到 stdout
                    printf("Echo: %s", buf);
                } else if (n == 0) {
                    printf("stdin EOF\n");
                    running = 0;
                    break;
                }
            } else if (events[i].data.fd == pipefd[0]) {
                // pipe 数据到达
                ssize_t n = read(pipefd[0], buf, sizeof(buf) - 1);
                if (n > 0) {
                    buf[n] = '\0';
                    printf("Pipe data: %s", buf);
                } else if (n == 0) {
                    printf("Pipe closed\n");
                }
            }
        }
    }

    close(epfd);
    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}
```

编译运行：
```bash
gcc -o epoll_demo epoll_demo.c
./epoll_demo
```

**关键点：**

**poll 方案：**
- 每次 `poll()` 需要传递所有 FD
- O(n) 扫描所有 FD 检查事件
- 适合 FD 数量较少的场景

**epoll 方案：**
- 使用 EPOLL_CTL_ADD 预先注册 FD
- O(1) 或 O(log n) 查询就绪事件
- 适合大量并发连接的场景
- Linux 专有，不可移植

**实际应用建议：**
- 对于文件/管道：`poll` 足够
- 对于网络编程（socket）：优先使用 `epoll`
- 要编写可移植代码：考虑 `select` 或 `poll`