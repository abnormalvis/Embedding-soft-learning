# lseek：移动文件偏移

`lseek(2)` 用于设置/查询“打开文件描述（open file description）”中的当前偏移量，主要用于常规文件的随机访问。

## 1. 头文件与函数原型

```c
#include <sys/types.h>
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
```

## 2. whence

- `SEEK_SET`：从文件开头算
- `SEEK_CUR`：从当前偏移算
- `SEEK_END`：从文件末尾算

## 3. 返回值

- 成功：返回新的偏移量（`off_t`）
- 失败：返回 `-1`，设置 `errno`

## 4. 文件洞（sparse file）

如果你把偏移跳到文件末尾之后再写入，中间的“空洞”区域不会真的占用磁盘块，读取时会返回 0 字节（逻辑上全 0）。

实验：

```c
int fd = open("sparse.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
lseek(fd, 1024 * 1024, SEEK_SET);
write(fd, "X", 1);
close(fd);
```

用 `ls -l` 看逻辑大小，用 `du -h` 看实际占用。

## 5. 常见 errno

- `ESPIPE`：FD 不支持 seek（例如 pipe、socket）
- `EINVAL`：`whence` 非法
- `EBADF`：FD 无效

## 6. 练习

写程序：对普通文件与管道分别调用 `lseek`，观察错误码差异。

### 参考实现

```c
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(void) {
    printf("=== Test 1: Regular File ===\n");
    
    // 创建一个测试文件
    int fd_file = open("/tmp/testfile.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_file == -1) {
        perror("open");
        return 1;
    }

    // 写一些数据
    write(fd_file, "Hello, World!", 13);

    // 尝试 lseek
    off_t pos = lseek(fd_file, 0, SEEK_SET);
    if (pos == -1) {
        printf("lseek failed: %s (errno=%d)\n", strerror(errno), errno);
    } else {
        printf("lseek SUCCESS: new position = %ld\n", pos);
    }

    close(fd_file);

    printf("\n=== Test 2: Pipe ===\n");

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    // 尝试在管道上进行 lseek
    errno = 0;
    pos = lseek(pipefd[0], 0, SEEK_SET);
    if (pos == -1) {
        printf("lseek FAILED (expected): %s (errno=%d)\n", strerror(errno), errno);
    } else {
        printf("lseek SUCCESS (unexpected!): new position = %ld\n", pos);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    printf("\n=== Error Code Explanation ===\n");
    printf("Regular file: lseek() 成功，因为常规文件支持随机访问\n");
    printf("Pipe:         lseek() 失败，errno=ESPIPE (29)，\"Illegal seek\"\n");
    printf("              因为管道是流式数据，不支持随机访问\n");

    return 0;
}
```

编译运行：
```bash
gcc -o lseek_test lseek_test.c
./lseek_test
```

**预期输出：**
```
=== Test 1: Regular File ===
lseek SUCCESS: new position = 0

=== Test 2: Pipe ===
lseek FAILED (expected): Illegal seek (errno=29)

=== Error Code Explanation ===
Regular file: lseek() 成功，因为常规文件支持随机访问
Pipe:         lseek() 失败，errno=ESPIPE (29)，"Illegal seek"
              因为管道是流式数据，不支持随机访问
```

**关键点：**
- 常规文件支持 `lseek()`，可以随意移动偏移量
- 管道、socket 等流式对象不支持 `lseek()`，会返回 `ESPIPE` 错误
- 在编写通用 IO 代码时，需要考虑不同对象的 `lseek()` 支持情况

