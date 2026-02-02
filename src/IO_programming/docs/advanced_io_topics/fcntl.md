# fcntl：文件描述符控制（非阻塞、CLOEXEC、文件锁）

`fcntl(2)` 是一个“多功能”系统调用，用于对 FD 做控制类操作：设置/获取 flags、设置 `FD_CLOEXEC`、以及记录锁（record lock）。

## 1. 头文件与函数原型

```c
#include <fcntl.h>

int fcntl(int fd, int cmd, ... /* arg */ );
```

常用 `cmd`：

- `F_GETFL` / `F_SETFL`：获取/设置“文件状态标志”（如 `O_NONBLOCK`、`O_APPEND`）。
- `F_GETFD` / `F_SETFD`：获取/设置“FD 标志”（如 `FD_CLOEXEC`）。
- `F_SETLK` / `F_SETLKW` / `F_GETLK`：记录锁。

## 2. O_NONBLOCK：设置非阻塞

典型流程：先 `F_GETFL` 再 OR 上 `O_NONBLOCK`。

```c
int flags = fcntl(fd, F_GETFL);
if (flags == -1) perror("fcntl(F_GETFL)");

if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    perror("fcntl(F_SETFL)");
```

行为：

- 对可读对象（pipe/socket/tty）：无数据时 `read()` 立刻失败，`errno = EAGAIN/EWOULDBLOCK`。
- 对可写对象：缓冲满时 `write()` 可能返回 `EAGAIN` 或短写。

非阻塞通常要配合 IO 多路复用（`poll/epoll`）。

## 3. FD_CLOEXEC：防止 exec 后 FD 泄漏

当进程 `fork()` 后 `execve()` 新程序时，默认 FD 会被继承。某些场景（启动子进程执行外部命令）不希望把内部 FD 泄漏过去。

- 创建时一次性：`open(..., O_CLOEXEC)`、`pipe2(..., O_CLOEXEC)`。
- 已有 FD：

```c
int fdflags = fcntl(fd, F_GETFD);
if (fdflags == -1) perror("F_GETFD");
if (fcntl(fd, F_SETFD, fdflags | FD_CLOEXEC) == -1) perror("F_SETFD");
```

## 4. 记录锁（advisory record lock）

`fcntl` 锁是“建议锁”（advisory）：只有遵守锁协议的进程才会配合。

### 4.1 struct flock

```c
#include <fcntl.h>

struct flock lk;
lk.l_type   = F_WRLCK;     // F_RDLCK/F_WRLCK/F_UNLCK
lk.l_whence = SEEK_SET;
lk.l_start  = 0;
lk.l_len    = 0;          // 0 表示锁到 EOF
```

### 4.2 加锁/解锁

- `F_SETLK`：不阻塞，拿不到锁就失败（`EACCES`/`EAGAIN`）。
- `F_SETLKW`：阻塞等待锁。

```c
if (fcntl(fd, F_SETLKW, &lk) == -1) {
    perror("fcntl(F_SETLKW)");
}

lk.l_type = F_UNLCK;
if (fcntl(fd, F_SETLK, &lk) == -1) {
    perror("fcntl(F_SETLK unlock)");
}
```

### 4.3 注意事项

- 锁与 FD 的生命周期有关：进程关闭相关 FD（或退出）会释放锁。
- 同一进程对同一文件的锁可能会被“合并/替换”，调试时要小心。
- 与 `flock(2)` 的区别：`flock` 是整文件锁，语义更简单；`fcntl` 是记录锁（可锁区间）。

## 5. 练习

1. 写一个程序：打开同一个文件，尝试 `F_SETLK` 加写锁；并发运行两次，观察第二个进程的错误码。
2. 把 stdin 设为非阻塞，用循环读取并在 `EAGAIN` 时 `poll` 等待。
