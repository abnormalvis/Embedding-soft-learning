# 探索 C 标准库缓冲（stdio buffering）

理解 stdio 缓冲能解释很多“我明明 printf 了为什么看不到”的现象。

## 1. 三种常见缓冲策略

- **全缓冲**：攒满一块再写（常见于普通文件）
- **行缓冲**：遇到 `\n` 刷新（常见于终端 stdout）
- **无缓冲**：每次调用尽快写出（常见于 stderr）

## 2. 复现实验：stdout 重定向后变全缓冲

现象：同一段程序，直接运行和重定向输出文件时表现不同。

```c
for (int i = 0; i < 5; i++) {
	printf("tick %d ", i);
	sleep(1);
}
```

- 直接运行：你可能逐步看到输出（取决于终端与换行）
- `./a.out > out.txt`：通常直到程序结束才写入（全缓冲）

解决：

- 加 `\n` 或 `fflush(stdout)`
- 或用 `setvbuf(stdout, NULL, _IONBF, 0)` 暂时关缓冲（调试用）

## 3. fork + 缓冲：重复输出经典坑

如果在 `fork()` 前 stdout 缓冲里已有内容，父子进程会各自继承一份缓冲，导致同一内容被写两次。

避免：

- `fork()` 前 `fflush(NULL)`
- 或输出到 stderr（通常无缓冲）

## 4. setvbuf 简介

```c
#include <stdio.h>

// mode: _IOFBF 全缓冲, _IOLBF 行缓冲, _IONBF 无缓冲
int setvbuf(FILE *stream, char *buf, int mode, size_t size);
```

一般不需要在业务代码里频繁调整缓冲，但在学习与调试时很有帮助。

## 5. 练习

1. 写一个程序：每秒 `printf(".")` 不换行；分别在终端与重定向到文件下观察输出。
2. 在 `fork()` 前 `printf("hello")` 不换行，然后 `fork()`，观察输出次数，加入 `fflush` 修复。

