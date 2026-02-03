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
### 参考实现

**练习 1: 不使用 SA_RESTART（需要手动处理 EINTR）**

```c
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int interrupted = 0;

void sigalrm_handler(int signum) {
    printf("  [SIGALRM received at timestamp]\n");
    interrupted++;
    if (interrupted >= 3) {
        printf("  [Stopping after 3 signals]\n");
        return;
    }
    alarm(1);  // 再设置 1 秒的闹钟
}

int main(void) {
    printf("Program 1: WITHOUT SA_RESTART\n");
    printf("Waiting for input (will be interrupted by SIGALRM every 1 second)...\n");
    printf("Type something or wait for 3 alarms:\n\n");

    // 安装信号处理器（不设置 SA_RESTART）
    struct sigaction sa;
    sa.sa_handler = sigalrm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;  // 不使用 SA_RESTART
    sigaction(SIGALRM, &sa, NULL);

    // 设置 1 秒的闹钟
    alarm(1);

    char buf[256];
    while (interrupted < 3) {
        printf("Calling read()...\n");
        ssize_t n = read(STDIN_FILENO, buf, sizeof(buf) - 1);
        
        if (n > 0) {
            buf[n] = '\0';
            printf("Got input: %s\n", buf);
            break;
        } else if (n == 0) {
            printf("EOF\n");
            break;
        } else {
            // n == -1
            if (errno == EINTR) {
                printf("  read() returned -1 with EINTR (need to retry)\n");
                continue;
            }
            perror("read");
            break;
        }
    }

    alarm(0);  // 取消闹钟
    return 0;
}
```

**练习 2: 使用 SA_RESTART（内核自动重试）**

```c
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int interrupted = 0;

void sigalrm_handler(int signum) {
    printf("  [SIGALRM received]\n");
    interrupted++;
    if (interrupted >= 3) {
        printf("  [Stopping after 3 signals]\n");
        return;
    }
    alarm(1);  // 再设置 1 秒的闹钟
}

int main(void) {
    printf("Program 2: WITH SA_RESTART\n");
    printf("Waiting for input (will be interrupted by SIGALRM every 1 second)...\n");
    printf("Type something or wait for 3 alarms:\n\n");

    // 安装信号处理器，启用 SA_RESTART
    struct sigaction sa;
    sa.sa_handler = sigalrm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;  // 启用自动重启
    sigaction(SIGALRM, &sa, NULL);

    // 设置 1 秒的闹钟
    alarm(1);

    char buf[256];
    while (interrupted < 3) {
        printf("Calling read()...\n");
        ssize_t n = read(STDIN_FILENO, buf, sizeof(buf) - 1);
        
        if (n > 0) {
            buf[n] = '\0';
            printf("Got input: %s\n", buf);
            break;
        } else if (n == 0) {
            printf("EOF\n");
            break;
        } else {
            // n == -1，此时几乎不会是 EINTR（被自动重启了）
            perror("read");
            break;
        }
    }

    alarm(0);  // 取消闹钟
    printf("\nNote: read() likely succeeded even with signals, thanks to SA_RESTART\n");
    return 0;
}
```

编译运行：
```bash
gcc -o eintr_test1 eintr_test1.c
gcc -o eintr_test2 eintr_test2.c

# 测试 1: 不使用 SA_RESTART
echo "test" | ./eintr_test1

# 测试 2: 使用 SA_RESTART
echo "test" | ./eintr_test2

# 或者用管道在后台运行
./eintr_test1 &
sleep 5; killall eintr_test1

./eintr_test2 &
sleep 5; killall eintr_test2
```

**对比输出：**

**Program 1（不使用 SA_RESTART）：**
```
Calling read()...
  [SIGALRM received]
  read() returned -1 with EINTR (need to retry)
Calling read()...
```

**Program 2（使用 SA_RESTART）：**
```
Calling read()...
  [SIGALRM received]
Calling read()...
  (read() 被内核自动重试，通常会立刻返回)
```

**关键点：**
- 不设置 `SA_RESTART` 时，阻塞系统调用被中断会返回 `EINTR`，需要手动重试
- 设置 `SA_RESTART` 后，内核会自动重启某些系统调用（但不是全部）
- 为了跨平台兼容性，最安全的做法是始终显式处理 `EINTR`，不完全依赖 `SA_RESTART`
- 信号处理器应该尽量简洁，避免在其中调用不安全的函数