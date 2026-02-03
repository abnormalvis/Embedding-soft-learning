# close：关闭文件描述符

`close(2)` 关闭一个 FD，释放进程对该内核对象的引用。

## 1. 头文件与函数原型

```c
#include <unistd.h>

int close(int fd);
```

## 2. 返回值

- 成功：返回 `0`
- 失败：返回 `-1`，设置 `errno`

## 3. 常见注意事项

1. **关闭后 FD 可能被复用**：
	- 你 `close(3)` 后，下一次 `open()` 很可能再次返回 `3`。
	- 所以不要在关闭后继续使用旧 FD。

2. **错误处理的现实主义**：
	- `close()` 也可能失败（例如网络文件系统、延迟写入等）。
	- 但很多程序在退出路径上会忽略 `close` 错误；关键场景（写入持久化要求）应检查。

3. **和 stdio 混用**：
	- 若你用 `fopen()` 得到 `FILE*`，应使用 `fclose()`，不要直接 `close(fileno(fp))`，否则缓冲未刷出。

## 4. 常见 errno

- `EBADF` fd 无效或已关闭
- `EINTR` 被信号中断（少见，但可重试）

## 5. 练习

写程序：`open()` 一个文件，`close()` 两次，观察第二次的 `errno`。

### 参考实现

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(void) {
    // 打开一个文件（/etc/passwd 通常存在）
    int fd = open("/etc/passwd", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("File opened successfully, FD: %d\n", fd);

    // 第一次关闭，应该成功
    if (close(fd) == -1) {
        perror("First close");
        return 1;
    }
    printf("First close: SUCCESS\n");

    // 第二次关闭同一个 FD，应该失败
    errno = 0;  // 清除之前的 errno
    if (close(fd) == -1) {
        printf("Second close: FAILED (expected)\n");
        printf("  errno: %d\n", errno);
        printf("  Error: %s\n", strerror(errno));
    } else {
        printf("Second close: SUCCESS (unexpected!)\n");
    }

    return 0;
}
```

编译运行：
```bash
gcc -o close_demo close_demo.c
./close_demo
```

**预期输出：**
```
File opened successfully, FD: 3
First close: SUCCESS
Second close: FAILED (expected)
  errno: 9
  Error: Bad file descriptor
```

**说明：**
- 第一次 `close()` 成功，FD 被释放
- 第二次 `close()` 失败，因为 FD 已经无效
- `errno` 为 `EBADF`（9），表示"Bad file descriptor"

