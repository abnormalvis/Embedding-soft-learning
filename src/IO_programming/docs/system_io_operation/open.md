# open：打开/创建文件

`open(2)` 用于打开文件并返回文件描述符（FD）。现代 Linux 上很多实现会走 `openat(2)`，但用法与概念一致。

## 1. 头文件与函数原型

```c
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```

当 `flags` 包含 `O_CREAT` 时必须提供 `mode`。

## 2. flags 常用组合

- 打开方式（三选一）：
	- `O_RDONLY` 只读
	- `O_WRONLY` 只写
	- `O_RDWR` 读写
- 常用附加标志：
	- `O_CREAT` 不存在则创建
	- `O_TRUNC` 截断为 0（通常与 `O_WRONLY`/`O_RDWR` 组合）
	- `O_APPEND` 追加写（每次 `write` 追加到文件末尾）
	- `O_EXCL` 与 `O_CREAT` 一起用：保证“创建时不存在”（常用于锁文件）
	- `O_CLOEXEC` 在 `execve` 时自动关闭（防止 FD 泄漏到子进程）

示例：

```c
int fd = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
```

## 3. 返回值

- 成功：返回非负 FD
- 失败：返回 `-1`，并设置 `errno`

## 4. 常见 errno

- `ENOENT` 路径不存在（或中间目录不存在）
- `EACCES` 权限不足
- `EISDIR` 试图以写方式打开目录
- `EMFILE` 进程打开文件数达到上限
- `ENFILE` 系统范围打开文件数达到上限

## 5. 示例：安全创建（避免覆盖）

```c
int fd = open("lock", O_WRONLY | O_CREAT | O_EXCL, 0644);
if (fd == -1) {
		// 如果 errno == EEXIST，说明已存在
}
```

## 6. 练习

1. 实现一个小程序：接受 `path flags mode`，打印打开成功的 FD 或失败原因。
2. 对比 `O_APPEND` 与普通写：多进程并发写入同一文件，观察输出差异。

### 参考实现

**练习 1: 打开文件程序**

```c
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int parse_flags(const char *flagstr) {
    int flags = 0;
    
    if (strchr(flagstr, 'r')) flags |= O_RDONLY;
    if (strchr(flagstr, 'w')) flags |= O_WRONLY;
    if (strchr(flagstr, '+')) flags = (flags & ~(O_RDONLY | O_WRONLY)) | O_RDWR;
    if (strchr(flagstr, 'c')) flags |= O_CREAT;
    if (strchr(flagstr, 't')) flags |= O_TRUNC;
    if (strchr(flagstr, 'a')) flags |= O_APPEND;
    if (strchr(flagstr, 'e')) flags |= O_EXCL;
    
    return flags;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <path> [flags] [mode]\n", argv[0]);
        fprintf(stderr, "  flags: r(read) w(write) +(rdwr) c(creat) t(trunc) a(append) e(excl)\n");
        fprintf(stderr, "  mode: octal (default 0644)\n");
        return 1;
    }

    const char *path = argv[1];
    const char *flagstr = argc > 2 ? argv[2] : "r";
    mode_t mode = argc > 3 ? (mode_t)strtol(argv[3], NULL, 8) : 0644;

    int flags = parse_flags(flagstr);
    
    printf("Opening: %s\n", path);
    printf("  flags: 0x%x\n", flags);
    printf("  mode: %o\n", mode);

    int fd;
    if (flags & O_CREAT) {
        fd = open(path, flags, mode);
    } else {
        fd = open(path, flags);
    }

    if (fd == -1) {
        printf("Failed: %s (errno=%d)\n", strerror(errno), errno);
        return 1;
    }

    printf("Success! FD: %d\n", fd);
    close(fd);
    return 0;
}
```

编译运行：
```bash
gcc -o open_demo open_demo.c
./open_demo /tmp/test.txt r              # 只读打开
./open_demo /tmp/test.txt w              # 只写打开（不存在会失败）
./open_demo /tmp/test.txt "wc" 0644      # 写+创建
./open_demo /tmp/test.txt "wce" 0644     # 写+创建+独占
```

**练习 2: O_APPEND 与普通写的对比**

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <normal|append> [num_processes]\n", argv[0]);
        return 1;
    }

    const char *mode = argv[1];
    int num_procs = argc > 2 ? atoi(argv[2]) : 5;
    const char *filename = "/tmp/write_test.txt";

    // 清空文件
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    close(fd);

    printf("Mode: %s, Processes: %d\n", mode, num_procs);

    for (int i = 0; i < num_procs; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // 子进程
            int flags = O_WRONLY;
            if (strcmp(mode, "append") == 0) {
                flags |= O_APPEND;
            }
            
            int fd = open(filename, flags);
            if (fd == -1) {
                perror("open");
                return 1;
            }

            // 写入标识
            char buf[32];
            snprintf(buf, sizeof(buf), "Process %d\n", i);
            
            for (int j = 0; j < 10; j++) {
                write(fd, buf, strlen(buf));
                usleep(10000);  // 模拟其他处理
            }

            close(fd);
            return 0;
        }
    }

    // 等待所有子进程
    for (int i = 0; i < num_procs; i++) {
        wait(NULL);
    }

    // 显示结果
    printf("\nFile contents:\n");
    system("cat " "/tmp/write_test.txt" " | head -20");
    printf("\n(总行数: ");
    system("wc -l " "/tmp/write_test.txt");
    
    return 0;
}
```

编译运行：
```bash
gcc -o append_demo append_demo.c
./append_demo normal 3      # 普通写入，观察混乱
./append_demo append 3      # O_APPEND 写入，更有序
```

**关键观察：**

- **普通写入**：多个进程会在文件中间竞争写位置，可能导致数据混乱和丢失
- **O_APPEND**：每次写入都原子地追加到文件末尾，虽然顺序不确定但不会丢数据

