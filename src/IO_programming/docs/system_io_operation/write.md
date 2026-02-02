# write：写入数据

`write(2)` 向 FD 对应对象写入字节序列，返回实际写入的字节数。

## 1. 头文件与函数原型

```c
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t count);
```

## 2. 返回值语义（必须掌握）

- `>= 0`：实际写入字节数（可能小于 `count`）
- `== -1`：失败，查看 `errno`

### 2.1 “短写”是正常现象

常见于：

- 非阻塞 FD（管道/socket）
- 信号中断
- 资源受限

所以写入固定长度数据要循环直到写完。

## 3. 可靠写入：write_all

```c
ssize_t write_all(int fd, const void *buf, size_t count) {
	const char *p = (const char *)buf;
	size_t left = count;
	while (left > 0) {
		ssize_t n = write(fd, p, left);
		if (n > 0) {
			p += n;
			left -= (size_t)n;
			continue;
		}
		if (n == -1 && errno == EINTR) {
			continue;
		}
		return -1;
	}
	return (ssize_t)count;
}
```

## 4. SIGPIPE 与 EPIPE

当向已关闭读端的管道/socket 写入时：

- 默认会收到 `SIGPIPE` 导致进程退出
- 系统调用返回 `-1`，`errno == EPIPE`

常见处理：

- 忽略 `SIGPIPE`（`signal(SIGPIPE, SIG_IGN)`）并检查 `EPIPE`
- 或用 `send(..., MSG_NOSIGNAL)`（socket 场景）

## 5. 练习

写一个“安全输出”函数，把任意字符串写到 stdout：要求处理短写与 `EINTR`。

