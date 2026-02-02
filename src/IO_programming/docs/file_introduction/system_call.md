# 系统调用（System Call）入门

系统调用是用户态程序进入内核态请求服务的入口。Linux 用户程序通常通过 C 标准库（glibc）提供的封装函数间接触发系统调用。

## 1. 为什么需要系统调用

内核掌管硬件与关键资源（进程调度、内存、文件系统、网络、设备）。用户态不能直接操作这些资源，只能：

1. 通过系统调用请求内核执行
2. 内核校验参数、权限与状态
3. 执行后返回结果与错误码

## 2. “库函数”与“系统调用”区别

常见误区是把所有 API 都叫系统调用。

- **系统调用**：由内核实现，例如 `openat(2)`、`read(2)`、`write(2)`、`close(2)`。
- **库函数**：由 libc 实现，可能内部调用系统调用，也可能纯用户态，例如 `printf(3)`、`malloc(3)`。

查看手册章号能快速区分：

- `man 2 read`：系统调用
- `man 3 printf`：库函数

## 3. 返回值与 errno

绝大多数系统调用在失败时返回 `-1`，并设置 `errno`。

典型写法：

```c
#include <errno.h>
#include <stdio.h>
#include <string.h>

// ...
if (fd == -1) {
	fprintf(stderr, "open failed: %s\n", strerror(errno));
}
```

要点：

- `errno` 只在失败时有意义。
- `perror("msg")` 会打印 `msg: <errno 对应字符串>`。

## 4. EINTR：被信号中断

阻塞型系统调用（例如 `read()` 等待数据）可能被信号打断，返回 `-1` 且 `errno == EINTR`。

处理策略：

- 对可重试的调用，循环重试
- 或使用 `sigaction()` + `SA_RESTART`（并非对所有调用都生效）

## 5. 观察系统调用：strace

学习 IO 编程时，`strace` 极其有用：

```bash
strace -f -o trace.log ./a.out
grep -E 'open|openat|read|write|close' trace.log | head
```

你可以直观看到：

- 你以为的 `fopen()` 实际上调用了 `openat()`
- `printf()` 什么时候才触发 `write()`（缓冲行为）

## 6. 推荐阅读顺序

1. `open(2)` / `close(2)`：拿到 FD、释放 FD
2. `read(2)` / `write(2)`：短读短写与循环
3. `lseek(2)`：偏移与文件洞
4. `stat(2)`：获取元信息
5. `access(2)`：权限判断的注意事项

