# 信号与 IO：EINTR、SA_RESTART 与可重试逻辑

阻塞型系统调用可能被信号中断，导致返回 `-1` 且 `errno == EINTR`。

## 1. 哪些调用容易被中断

常见：

- `read`（阻塞等待输入）
- `write`（少见，但可能）
- `accept` / `connect` / `select` / `poll` / `epoll_wait`
- `nanosleep`

## 2. 正确处理模式：可重试就重试

对于“语义允许重试”的调用，最通用的写法是：

```c
ssize_t n;
while ((n = read(fd, buf, len)) == -1 && errno == EINTR) {
    // retry
}
```

对 `poll/epoll_wait` 也一样。

## 3. SA_RESTART：让内核自动重启（但不是万能）

用 `sigaction` 安装信号处理器时，可设置 `SA_RESTART` 让部分被中断的系统调用自动重启。

```c
#include <signal.h>

struct sigaction sa = {0};
sa.sa_handler = handler;
sa.sa_flags = SA_RESTART;

sigaction(SIGINT, &sa, NULL);
```

注意：

- 并非所有系统调用都能被自动重启。
- 即使能重启，你仍需要写健壮的 `EINTR` 处理（尤其是可移植代码）。

## 4. 练习

1. 写程序：阻塞 `read` stdin，同时用 `alarm(1)` 周期性触发信号；观察 `EINTR`。
2. 开启 `SA_RESTART` 后再观察，比较差异。
