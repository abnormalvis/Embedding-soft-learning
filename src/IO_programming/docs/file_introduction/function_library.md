# C 函数库与 IO：libc 视角

Linux 上常见的 C 标准库实现是 glibc。IO 学习中你会同时遇到两套接口：

- **系统调用 IO（POSIX）**：`open/read/write/close`，操作对象是 FD。
- **标准 IO（stdio）**：`fopen/fread/fwrite/fclose/printf`，操作对象是 `FILE*`，有用户态缓冲。

## 1. 为什么有 stdio（FILE*）

stdio 解决的核心问题：

- 通过缓冲减少系统调用次数（提升性能）
- 提供更友好的格式化输出（`printf` 家族）
- 提供跨平台一致接口

代价：

- 多一层缓冲与状态，调试时要理解“数据还在用户态缓冲区里”
- 与 `fork()`、多进程/多线程混用时更容易踩坑

## 2. `FILE*` 与 FD 的关系

常见转换：

- `fileno(FILE *stream)`：从 `FILE*` 得到 FD
- `fdopen(int fd, const char *mode)`：把已有 FD 包装成 `FILE*`

注意：

- 同一个底层 FD 被多个 `FILE*` 包装时，缓冲可能相互干扰（不推荐）。

## 3. 缓冲类型

stdio 常见缓冲模式：

- 全缓冲（full buffered）：通常用于普通文件
- 行缓冲（line buffered）：通常用于终端（遇到 `\n` 刷新）
- 无缓冲（unbuffered）：通常 stderr

可通过：

- `setvbuf()` / `setbuf()` 调整
- `fflush()` 主动刷新输出缓冲

## 4. 手册章号小抄

- `man 2 xxx`：系统调用
- `man 3 xxx`：库函数
- `man 7 xxx`：概念说明（如 `man 7 signal`、`man 7 pipe`）

## 5. 学习建议

1. 先把系统调用 IO（FD）吃透：错误处理、短读短写、偏移。
2. 再学习 stdio：缓冲、格式化、`FILE*` 与 FD 转换。
3. 最后进入“高级 IO”：非阻塞、复用（select/poll/epoll）、目录遍历、mmap、管道等。

