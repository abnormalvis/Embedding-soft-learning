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
### 参考实现

```c
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

// 安全写函数：确保所有数据都被写出
ssize_t safe_write(int fd, const void *buf, size_t count) {
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
            // 被信号中断，重试
            continue;
        }
        
        if (n == -1) {
            // 真正的错误
            return -1;
        }
    }
    
    return (ssize_t)count;
}

int main(void) {
    // 测试 1: 简单字符串
    const char *str1 = "Hello, World!\n";
    if (safe_write(STDOUT_FILENO, str1, strlen(str1)) == -1) {
        perror("safe_write");
        return 1;
    }
    
    // 测试 2: 较长字符串（模拟可能发生短写的场景）
    const char *str2 = "This is a longer string to test safe_write function.\n";
    if (safe_write(STDOUT_FILENO, str2, strlen(str2)) == -1) {
        perror("safe_write");
        return 1;
    }
    
    // 测试 3: 二进制数据
    unsigned char binary[] = {0x01, 0x02, 0x03, 0x04, 0x0A};
    if (safe_write(STDOUT_FILENO, binary, sizeof(binary)) == -1) {
        perror("safe_write");
        return 1;
    }
    
    return 0;
}
```

编译运行：
```bash
gcc -o safe_write safe_write.c
./safe_write
```

**关键点说明：**

- `safe_write()` 循环写入直到所有数据都写完或遇到不可恢复错误
- 处理短写：即使 `write()` 只写了部分数据，继续写剩余部分
- 处理 `EINTR`：被信号中断时重试 `write()`
- 返回值为写入的总字节数（正常情况）或 `-1`（错误）

这样的函数在实现数据可靠传输时非常重要。
