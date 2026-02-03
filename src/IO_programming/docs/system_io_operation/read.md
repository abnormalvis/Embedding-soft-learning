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

### 参考实现

```c
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char buf[4096];
    
    for (;;) {
        ssize_t n;
        
        // 循环处理 EINTR
        do {
            n = read(STDIN_FILENO, buf, sizeof(buf));
        } while (n == -1 && errno == EINTR);
        
        // 处理各种返回情况
        if (n > 0) {
            // 成功读取，写到 stdout
            ssize_t written = 0;
            while (written < n) {
                ssize_t w = write(STDOUT_FILENO, buf + written, n - written);
                if (w == -1) {
                    if (errno == EINTR) continue;
                    perror("write");
                    return 1;
                }
                written += w;
            }
        } else if (n == 0) {
            // EOF，正常退出
            return 0;
        } else {
            // 错误
            perror("read");
            return 1;
        }
    }
    
    return 0;
}
```

编译运行：
```bash
gcc -o simple_cat simple_cat.c
# 从 stdin 读取并打印到 stdout
echo "Hello, World!" | ./simple_cat
# 或直接读取文件
./simple_cat < /etc/passwd
```

**关键点说明：**

- 循环处理 `EINTR` 确保被信号中断时自动重试
- 处理 `n == 0` 表示 EOF，此时应正常退出
- 写入时也应处理短写（虽然在 stdout 上少见，但为了健壮性应该这样写）

