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
