# pread/pwrite：原子随机读写（不改变文件偏移）

`pread(2)` / `pwrite(2)` 在指定偏移进行读写，但**不会改变 FD 当前偏移量**。它们在多线程/并发场景中非常实用。

## 1. 头文件与函数原型

```c
#include <unistd.h>

ssize_t pread(int fd, void *buf, size_t count, off_t offset);
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);
```

## 2. 为什么说“原子”

这里的“原子”主要指：

- 对同一次调用来说，偏移与读写在内核里作为一个操作完成
- 不需要 `lseek + read/write` 的组合，因此避免线程间偏移互相干扰

并不意味着多次调用不会交错。

## 3. 使用场景

- 多线程读取同一文件不同区间
- 实现文件格式解析（随机访问）
- 实现简单 KV 存储（固定偏移读写）

## 4. 注意事项

- 仍需处理短读短写与 `EINTR`。
- 对非 seekable 的 FD（pipe/socket）通常不适用。

## 5. 练习

写一个程序：启动 N 个线程，对同一文件不同 offset 做 `pread`，比较与 `lseek+read` 的实现复杂度差异。
