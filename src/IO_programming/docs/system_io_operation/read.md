# read：读取数据

`read(2)` 从 FD 对应的对象读取字节序列，返回实际读取的字节数。

## 1. 头文件与函数原型

```c
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count);
```

## 2. 返回值语义（非常重要）

- `> 0`：成功读取的字节数
- `== 0`：到达 EOF（对普通文件）或对端关闭（对 pipe/socket）
- `== -1`：失败，查看 `errno`

### 2.1 “短读”很正常

即使 `count` 很大，`read()` 也可能只返回一部分数据：

- 对终端、管道、socket：按可用数据返回
- 被信号打断：`errno == EINTR`
- 文件到尾部：剩余不足 `count`

## 3. 可靠读取：处理 EINTR

```c
ssize_t n;
do {
	n = read(fd, buf, count);
} while (n == -1 && errno == EINTR);
```

如果需要“读满 count 字节”，就要写循环累计（对常规文件可行；对流式输入不一定有“读满”的语义）。

## 4. 常见 errno

- `EBADF` fd 不可读或无效
- `EINTR` 被信号中断
- `EAGAIN`/`EWOULDBLOCK` 非阻塞且暂时无数据
- `EFAULT` 缓冲区地址非法

## 5. 练习

实现简化版 `cat`：从 stdin（fd=0）循环 `read`，写到 stdout（fd=1）。要求正确处理 `EINTR`。

