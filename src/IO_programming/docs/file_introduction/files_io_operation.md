# Linux 文件 IO 操作总览

本章给出 IO 编程的“地图”，帮助你把后续各个 API 串成一条线。

## 1. IO 的两套接口

### 1.1 系统调用 IO（FD）

- API：`open/close/read/write/lseek/stat/...`
- 特点：更贴近内核，语义清晰，适合实现底层工具与库
- 常见问题：短读短写、`EINTR`、阻塞/非阻塞

### 1.2 标准 IO（FILE*）

- API：`fopen/fclose/fread/fwrite/fseek/ftell/printf/...`
- 特点：带缓冲，使用更方便，性能通常更好
- 常见问题：缓冲刷新时机、与 `fork()` 混用

## 2. 阻塞与非阻塞

- **阻塞 IO**：没有数据/资源时，系统调用睡眠等待（例如 `read()` 等待输入）。
- **非阻塞 IO**：立即返回，常见错误是 `EAGAIN`/`EWOULDBLOCK`。

非阻塞通常通过 `fcntl(fd, F_SETFL, O_NONBLOCK)` 设置，后续配合 IO 多路复用（select/poll/epoll）。

## 3. 短读短写（必须掌握）

- `read()` 可能返回小于请求长度的字节数（EOF、被信号中断、管道/终端等）。
- `write()` 也可能只写入部分数据（尤其是非阻塞或写入管道/socket）。

因此可靠写法是循环直到完成或遇到不可恢复错误。

## 4. 文件偏移与随机访问

- 顺序 IO：直接 `read/write`，内核维护并推进偏移
- 随机访问：`lseek()` 设置偏移后再读写
- 原子随机访问：`pread()/pwrite()`（不改变文件描述的当前偏移）

## 5. 元信息与权限判断

- `stat()` 获取文件类型、权限、大小、时间戳
- `access()` 检查“以真实 UID/GID 是否有权限访问”，但存在 TOCTOU 风险（检查后到使用前状态可能变化）

## 6. 排查与调试

- 用 `strace` 看系统调用序列
- 用 `errno`/`perror`/`strerror` 输出明确错误
- 多用 `man 2` / `man 3` 看权威说明

## 7. 推荐练习路线

1. 写 `cp`：实现文件拷贝（循环 `read` + `write`）
2. 写 `cat`：支持从 stdin 到 stdout
3. 写 `seek-demo`：展示 `lseek` 与文件洞
4. 写 `stat-demo`：打印文件类型与权限
5. 再进入 stdio 与目录相关 API

### 参考实现

**练习 1: cp - 文件拷贝**

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE 4096

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <src> <dst>\n", argv[0]);
        return 1;
    }

    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd == -1) {
        perror("open src");
        return 1;
    }

    int dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd == -1) {
        perror("open dst");
        close(src_fd);
        return 1;
    }

    char buf[BUF_SIZE];
    ssize_t n;
    
    while ((n = read(src_fd, buf, BUF_SIZE)) > 0) {
        if (write(dst_fd, buf, n) != n) {
            perror("write");
            close(src_fd);
            close(dst_fd);
            return 1;
        }
    }

    if (n < 0) {
        perror("read");
        close(src_fd);
        close(dst_fd);
        return 1;
    }

    close(src_fd);
    close(dst_fd);
    printf("Copied: %s -> %s\n", argv[1], argv[2]);
    
    return 0;
}
```

**练习 2: cat - 标准输入/输出**

```c
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 4096

int cat_fd(int fd) {
    char buf[BUF_SIZE];
    ssize_t n;
    
    while ((n = read(fd, buf, BUF_SIZE)) > 0) {
        write(STDOUT_FILENO, buf, n);
    }
    
    return (n < 0) ? 1 : 0;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        // 无参数：从 stdin 读取
        return cat_fd(STDIN_FILENO);
    }

    // 有参数：打开文件列表
    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd == -1) {
            perror(argv[i]);
            continue;
        }
        cat_fd(fd);
        close(fd);
    }
    
    return 0;
}
```

**练习 3: seek-demo - 文件洞演示**

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main(void) {
    const char *filename = "/tmp/sparse_file.bin";
    
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // 写入 "START"
    write(fd, "START", 5);
    
    // 跳到 1MB 处
    lseek(fd, 1024 * 1024, SEEK_SET);
    
    // 写入 "END"
    write(fd, "END", 3);
    
    close(fd);

    // 查看文件大小
    struct stat st;
    stat(filename, &st);
    
    printf("Logical size: %ld bytes\n", st.st_size);
    
    // 使用 du 查看实际占用
    printf("Actual usage:\n");
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "du -h %s && ls -lh %s", filename, filename);
    system(cmd);
    
    return 0;
}
```

编译运行这些练习：
```bash
gcc -o simple_cp simple_cp.c
gcc -o simple_cat simple_cat.c
gcc -o seek_demo seek_demo.c

# 测试 cp
./simple_cp /etc/passwd /tmp/passwd_copy

# 测试 cat
./simple_cat /etc/passwd | head -5
echo "test" | ./simple_cat

# 测试 seek
./seek_demo
```

