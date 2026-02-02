# open：打开/创建文件

`open(2)` 用于打开文件并返回文件描述符（FD）。现代 Linux 上很多实现会走 `openat(2)`，但用法与概念一致。

## 1. 头文件与函数原型

```c
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```

当 `flags` 包含 `O_CREAT` 时必须提供 `mode`。

## 2. flags 常用组合

- 打开方式（三选一）：
	- `O_RDONLY` 只读
	- `O_WRONLY` 只写
	- `O_RDWR` 读写
- 常用附加标志：
	- `O_CREAT` 不存在则创建
	- `O_TRUNC` 截断为 0（通常与 `O_WRONLY`/`O_RDWR` 组合）
	- `O_APPEND` 追加写（每次 `write` 追加到文件末尾）
	- `O_EXCL` 与 `O_CREAT` 一起用：保证“创建时不存在”（常用于锁文件）
	- `O_CLOEXEC` 在 `execve` 时自动关闭（防止 FD 泄漏到子进程）

示例：

```c
int fd = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
```

## 3. 返回值

- 成功：返回非负 FD
- 失败：返回 `-1`，并设置 `errno`

## 4. 常见 errno

- `ENOENT` 路径不存在（或中间目录不存在）
- `EACCES` 权限不足
- `EISDIR` 试图以写方式打开目录
- `EMFILE` 进程打开文件数达到上限
- `ENFILE` 系统范围打开文件数达到上限

## 5. 示例：安全创建（避免覆盖）

```c
int fd = open("lock", O_WRONLY | O_CREAT | O_EXCL, 0644);
if (fd == -1) {
		// 如果 errno == EEXIST，说明已存在
}
```

## 6. 练习

1. 实现一个小程序：接受 `path flags mode`，打印打开成功的 FD 或失败原因。
2. 对比 `O_APPEND` 与普通写：多进程并发写入同一文件，观察输出差异。

