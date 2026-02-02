# dup/dup2/dup3：复制文件描述符与重定向原理

复制 FD 的核心作用：让多个 FD 指向同一个“打开文件描述（open file description）”，从而共享偏移与状态；同时它也是 shell 重定向（`>`, `2>`, `2>&1`）的底层机制。

## 1. 头文件与函数原型

```c
#include <unistd.h>

int dup(int oldfd);
int dup2(int oldfd, int newfd);

// Linux: dup3
#include <fcntl.h>
int dup3(int oldfd, int newfd, int flags); // flags 可用 O_CLOEXEC
```

## 2. 行为要点

- `dup(oldfd)`：返回一个**最小可用**的新 FD。
- `dup2(oldfd, newfd)`：让 `newfd` 指向 `oldfd` 同一打开实例。
  - 若 `newfd` 已打开，会先关闭它（原子地完成“关闭 + 复制”）。
  - 若 `oldfd == newfd`，直接返回 `newfd`。
- `dup3` 类似 `dup2`，但可设置 `O_CLOEXEC`，且 `oldfd == newfd` 会报错。

## 3. 为什么会共享偏移

`dup` 后两个 FD 共享同一打开实例：

- `lseek` 影响彼此
- `read`/`write` 推进的偏移影响彼此

用它能实现：把 stdout 重定向到文件、把 stderr 合并到 stdout 等。

## 4. 示例：把 stdout 重定向到文件

```c
int fd = open("out.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644);
if (fd == -1) perror("open");

if (dup2(fd, STDOUT_FILENO) == -1) perror("dup2");
close(fd);

printf("this goes to out.txt\n");
fflush(stdout);
```

说明：`dup2` 之后，FD=1（stdout）指向了 `out.txt`。

## 5. 常见坑

- 混用 stdio：重定向后用 `printf` 记得 `fflush`。
- 忘了设置 `CLOEXEC`：启动子进程执行外部程序时可能泄漏 FD。

## 6. 练习

1. 写程序实现 `2>&1`：让 stderr 合并到 stdout。
2. `dup` 一个文件 FD 后，分别 `read` 10 字节，观察偏移共享。
