# Linux 文件结构与文件描述符

在 Linux 中，“一切皆文件”更多是一种抽象：**进程通过文件描述符（file descriptor, FD）引用内核对象**，再由内核把这些对象映射到具体的文件、管道、socket、设备等。

## 1. 文件对象与 VFS（概念层）

Linux 通过 VFS（Virtual File System）为不同文件系统（ext4、xfs、procfs…）提供统一接口。

从进程视角常见的三层关系：

- **FD（int）**：进程私有的一个小整数索引。
- **打开文件描述（open file description）**：内核维护的“打开实例”（偏移量、状态标志、锁等）。多个 FD 可共享同一打开实例（如 `dup()`、`fork()` 继承）。
- **inode / dentry**：代表文件系统中的文件实体（权限、类型、大小、时间戳…）及路径名解析缓存。

理解这一点很关键：

- `lseek()` 修改的是“打开文件描述”的偏移量，所以共享同一打开实例的 FD 会相互影响。
- `open()` 打开同一路径两次通常会得到两个打开实例，因此偏移量互不影响。

## 2. 文件类型（stat 看见的那部分）

常见文件类型（`st_mode` 高位宏判断）：

- 普通文件：`S_ISREG(mode)`
- 目录：`S_ISDIR(mode)`
- 字符设备：`S_ISCHR(mode)`
- 块设备：`S_ISBLK(mode)`
- FIFO（命名管道）：`S_ISFIFO(mode)`
- socket：`S_ISSOCK(mode)`
- 符号链接：`S_ISLNK(mode)`

提示：`stat()` 会跟随符号链接；要查看链接本身用 `lstat()`。

## 3. 权限与 umask

权限常写为三组 `rwx`：owner/group/others。

- 读 `r`：文件可读；目录可列出条目名（需要 `x` 才能进入）。
- 写 `w`：文件可写；目录可创建/删除/重命名（仍需 `x` 才能访问条目）。
- 执行/搜索 `x`：文件可执行；目录可进入/可查找。

创建文件时 `open(..., mode)` 的 `mode` 会受 `umask` 影响：

$$\text{final\_mode} = \text{mode} \;\&\; \sim\text{umask}$$

## 4. 文件偏移量与原子性（重要）

- **偏移量**：`read()` / `write()` 默认使用并推进文件偏移。
- **原子性**：
	- 对同一 FD（共享打开实例）并发写入，应用层仍可能交错，除非使用 `O_APPEND` 或显式锁。
	- 对常规文件使用 `O_APPEND` 时，每次 `write()` 会在内核中原子地把偏移设到 EOF 后再写（但多次 `write()` 仍是多次操作）。

## 5. 标准输入/输出/错误

约定：

- `0`：stdin
- `1`：stdout
- `2`：stderr

它们只是默认已打开的 FD，完全可以被重定向（shell 的 `>`/`2>`/`<`）。

## 6. 实用排查命令

- 查看进程打开了哪些 FD：`ls -l /proc/<pid>/fd`
- 查看文件类型/权限：`stat file`，或 `ls -l`
- 观察系统调用：`strace -f -o trace.log ./your_program`

## 7. 小练习

1. 写一个程序：`open()` 一个文件两次，分别 `read()` 10 字节，比较两次读到的内容与偏移量变化。
2. `fork()` 后父子进程共享 FD：各自 `write()` 多次，观察输出交错；再对比 `O_APPEND` 的行为。

### 参考实现

**练习 1: 验证偏移量独立性**

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    printf("=== Open same file twice, separate offsets ===\n");
    
    // 第一次打开
    int fd1 = open("/etc/passwd", O_RDONLY);
    if (fd1 == -1) {
        perror("open fd1");
        return 1;
    }
    
    // 第二次打开（同一个文件）
    int fd2 = open("/etc/passwd", O_RDONLY);
    if (fd2 == -1) {
        perror("open fd2");
        return 1;
    }

    char buf1[11] = {0};
    char buf2[11] = {0};

    // 通过 fd1 读取 10 字节
    read(fd1, buf1, 10);
    printf("fd1 read (0-9):   %s\n", buf1);

    // 通过 fd2 读取 10 字节
    read(fd2, buf2, 10);
    printf("fd2 read (0-9):   %s\n", buf2);

    // 再通过 fd1 读取 10 字节
    read(fd1, buf1, 10);
    printf("fd1 read (10-19): %s\n", buf1);

    printf("\nConclusion: Each FD has independent offset\n");

    close(fd1);
    close(fd2);

    return 0;
}
```

**练习 2: fork 后的 FD 偏移共享与 O_APPEND 对比**

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <normal|append>\n", argv[0]);
        return 1;
    }

    const char *mode = argv[1];
    const char *filename = "/tmp/fork_write_test.txt";

    // 清空文件
    int init_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    close(init_fd);

    int flags = O_WRONLY;
    if (strcmp(mode, "append") == 0) {
        flags |= O_APPEND;
    }

    int fd = open(filename, flags);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // 子进程：写入 10 次
        for (int i = 0; i < 10; i++) {
            char buf[32];
            snprintf(buf, sizeof(buf), "Child-%d\n", i);
            write(fd, buf, strlen(buf));
            usleep(50000);  // 50ms 延迟，让父进程也有机会写
        }
    } else {
        // 父进程：写入 10 次
        for (int i = 0; i < 10; i++) {
            char buf[32];
            snprintf(buf, sizeof(buf), "Parent-%d\n", i);
            write(fd, buf, strlen(buf));
            usleep(40000);  // 40ms 延迟
        }
        
        wait(NULL);  // 等待子进程
    }

    close(fd);

    printf("Mode: %s\n", mode);
    printf("File contents:\n");
    system("cat " "/tmp/fork_write_test.txt");

    return 0;
}
```

编译运行：
```bash
gcc -o fd_offset fd_offset.c
./fd_offset

gcc -o fork_write fork_write.c
./fork_write normal   # 观察输出混乱
./fork_write append   # 观察输出虽混乱但不丢数据
```

**关键观察：**

**练习 1：**
- 两次 `open()` 的同一个文件会得到两个不同的 FD
- 这两个 FD 有**独立的**偏移量
- 修改 fd1 的偏移不会影响 fd2

**练习 2：**
- **普通写入（normal）：** 父子进程共享偏移，导致数据可能丢失或混乱
- **追加写入（O_APPEND）：** 每次 `write()` 都原子地追加到末尾，虽然顺序不定但不会丢数据

**示意图：**

普通写入的问题：
```
Parent: lseek -> offset=0, write 7 bytes -> offset=7
Child:  lseek -> offset=7, write 7 bytes -> offset=14
结果：可能互相覆盖
```

O_APPEND 的优势：
```
Parent: write with APPEND -> 原子地追加到当前末尾
Child:  write with APPEND -> 原子地追加到当前末尾
结果：两个数据都保存，虽然顺序不定
```

