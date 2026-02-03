# dup/dup2/dup3：复制文件描述符与重定向原理

复制 FD 的核心作用：让多个 FD 指向同一个“打开文件描述（open file description）”，从而共享偏移与状态；同时它也是 shell 重定向（`>`, `2>`, `2>&1`）的底层机制。

## 1. 头文件与函数原型

```c
#include <unistd.h>

int dup(int oldfd);
int dup2(int oldfd, int newfd);

// Linux: dup3
#include <fcntl.h>
int dup3(int oldfd, int newfd, int flags); // flags 可用 O_CLOEXEC
```

## 2. 行为要点

- `dup(oldfd)`：返回一个**最小可用**的新 FD。
- `dup2(oldfd, newfd)`：让 `newfd` 指向 `oldfd` 同一打开实例。
  - 若 `newfd` 已打开，会先关闭它（原子地完成“关闭 + 复制”）。
  - 若 `oldfd == newfd`，直接返回 `newfd`。
- `dup3` 类似 `dup2`，但可设置 `O_CLOEXEC`，且 `oldfd == newfd` 会报错。

## 3. 为什么会共享偏移

`dup` 后两个 FD 共享同一打开实例：

- `lseek` 影响彼此
- `read`/`write` 推进的偏移影响彼此

用它能实现：把 stdout 重定向到文件、把 stderr 合并到 stdout 等。

## 4. 示例：把 stdout 重定向到文件

```c
int fd = open("out.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644);
if (fd == -1) perror("open");

if (dup2(fd, STDOUT_FILENO) == -1) perror("dup2");
close(fd);

printf("this goes to out.txt\n");
fflush(stdout);
```

说明：`dup2` 之后，FD=1（stdout）指向了 `out.txt`。

## 5. 常见坑

- 混用 stdio：重定向后用 `printf` 记得 `fflush`。
- 忘了设置 `CLOEXEC`：启动子进程执行外部程序时可能泄漏 FD。

## 6. 练习

1. 写程序实现 `2>&1`：让 stderr 合并到 stdout。
2. `dup` 一个文件 FD 后，分别 `read` 10 字节，观察偏移共享。
### 参考实现

**练习 1: 实现 `2>&1` 重定向**

```c
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main(void) {
    printf("This is stdout\n");
    fprintf(stderr, "This is stderr (before dup2)\n");
    fflush(stdout);
    fflush(stderr);

    // 让 stderr (FD 2) 重定向到 stdout (FD 1)
    // 相当于 shell 中的 2>&1
    if (dup2(STDOUT_FILENO, STDERR_FILENO) == -1) {
        perror("dup2");
        return 1;
    }

    printf("This is stdout (after dup2)\n");
    fprintf(stderr, "This is stderr (after dup2, now merged to stdout)\n");

    fflush(stdout);
    fflush(stderr);

    return 0;
}
```

编译运行：
```bash
gcc -o dup_redirect dup_redirect.c
./dup_redirect
./dup_redirect 2>&1  # 效果相同
```

**预期输出：**
```
This is stdout
This is stderr (before dup2)
This is stdout (after dup2)
This is stderr (after dup2, now merged to stdout)
```

**练习 2: 观察偏移共享**

```c
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    // 创建测试文件
    int fd1 = open("/tmp/dup_test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(fd1, "0123456789ABCDEFGHIJ", 20);
    close(fd1);

    printf("=== Test: dup() offset sharing ===\n");

    // 打开文件用于读取
    int fd_orig = open("/tmp/dup_test.txt", O_RDONLY);
    if (fd_orig == -1) {
        perror("open");
        return 1;
    }

    // 复制 FD
    int fd_dup = dup(fd_orig);
    if (fd_dup == -1) {
        perror("dup");
        return 1;
    }

    printf("Original FD: %d, Duplicated FD: %d\n", fd_orig, fd_dup);

    // 通过原 FD 读取 10 字节
    char buf1[11] = {0};
    read(fd_orig, buf1, 10);
    printf("read(fd_orig, 10): \"%s\"\n", buf1);

    // 再从重复的 FD 读取 10 字节
    // 因为偏移共享，应该从第 10 字节开始读
    char buf2[11] = {0};
    read(fd_dup, buf2, 10);
    printf("read(fd_dup, 10): \"%s\"\n", buf2);

    printf("\nNote: buf2 从 'ABCDEFGHIJ' 开始，说明偏移被 fd_orig 的读操作推进了\n");

    close(fd_orig);
    close(fd_dup);

    return 0;
}
```

编译运行：
```bash
gcc -o dup_offset dup_offset.c
./dup_offset
```

**预期输出：**
```
=== Test: dup() offset sharing ===
Original FD: 3, Duplicated FD: 4
read(fd_orig, 10): "0123456789"
read(fd_dup, 10): "ABCDEFGHIJ"

Note: buf2 从 'ABCDEFGHIJ' 开始，说明偏移被 fd_orig 的读操作推进了
```

**关键点：**
- `dup2()` 可用于文件重定向，这是 shell 重定向的底层机制
- 多个 FD 共享同一打开实例时，文件偏移是共享的
- 这对于实现复杂的 IO 操作很重要