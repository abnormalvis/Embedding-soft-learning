# pipe/FIFO：管道与命名管道

管道用于进程间通信（IPC），数据是字节流。

## 1. pipe：匿名管道

### 1.1 原型

```c
#include <unistd.h>

int pipe(int pipefd[2]); // pipefd[0] 读端, pipefd[1] 写端
```

### 1.2 常见用法：父子进程通信

- `fork()` 后父子共享一对 FD
- 关闭不使用的一端：
  - 子进程只写：关闭读端
  - 父进程只读：关闭写端

EOF 规则：当**所有写端**都关闭后，读端 `read` 返回 0。

### 1.3 SIGPIPE

向没有读端的管道写：默认触发 `SIGPIPE`，系统调用返回 `EPIPE`。

## 2. FIFO：命名管道

FIFO 是文件系统中的一个特殊文件（`mkfifo` 创建），可用于无亲缘关系进程通信。

### 2.1 创建

```c
#include <sys/stat.h>

int mkfifo(const char *pathname, mode_t mode);
```

或者命令行：`mkfifo myfifo`

### 2.2 打开行为（阻塞点）

- 只读打开 FIFO：若没有写端，`open` 可能阻塞
- 只写打开 FIFO：若没有读端，`open` 可能阻塞

可用 `O_NONBLOCK` 改变行为。

## 3. 练习

1. 用 `pipe` 写一个“父进程读、子进程写”的 demo。
2. 用 FIFO 写一个简易聊天：A 写 FIFO，B 读 FIFO（反向也再做一根 FIFO）。

### 参考实现

**练习 1: pipe 实现父读子写**

```c
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        close(pipefd[0]);
        const char *messages[] = {
            "Hello from child\n",
            "This is message 2\n",
            "Goodbye!\n"
        };
        for (int i = 0; i < 3; i++) {
            write(pipefd[1], messages[i], strlen(messages[i]));
            sleep(1);
        }
        close(pipefd[1]);
        return 0;
    } else {
        close(pipefd[1]);
        char buf[256];
        ssize_t n;
        printf("Parent waiting for data from child...\n");
        while ((n = read(pipefd[0], buf, sizeof(buf) - 1)) > 0) {
            buf[n] = '\0';
            printf("Parent received: %s", buf);
        }
        close(pipefd[0]);
        wait(NULL);
        printf("Parent: child process terminated\n");
    }
    return 0;
}
```

**练习 2: 使用 FIFO 的简易聊天**

进程A：
```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main(void) {
    mkfifo("/tmp/fifo_a_to_b", 0666);
    mkfifo("/tmp/fifo_b_to_a", 0666);
    printf("Process A: Waiting for B...\n");
    int fd_out = open("/tmp/fifo_a_to_b", O_WRONLY);
    int fd_in = open("/tmp/fifo_b_to_a", O_RDONLY);
    printf("Connected!\n");
    char buf[256];
    for (int i = 0; i < 3; i++) {
        snprintf(buf, sizeof(buf), "Message %d from A\n", i);
        write(fd_out, buf, strlen(buf));
        printf("A sent: %s", buf);
        ssize_t n = read(fd_in, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("A received: %s", buf);
        }
    }
    close(fd_out);
    close(fd_in);
    return 0;
}
```

进程B：
```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main(void) {
    printf("Process B: Waiting for A...\n");
    int fd_in = open("/tmp/fifo_a_to_b", O_RDONLY);
    int fd_out = open("/tmp/fifo_b_to_a", O_WRONLY);
    printf("Connected!\n");
    char buf[256];
    for (int i = 0; i < 3; i++) {
        ssize_t n = read(fd_in, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("B received: %s", buf);
        }
        snprintf(buf, sizeof(buf), "Reply %d from B\n", i);
        write(fd_out, buf, strlen(buf));
        printf("B sent: %s", buf);
    }
    close(fd_in);
    close(fd_out);
    return 0;
}
```

编译运行：
```bash
gcc -o fifo_chat_a fifo_chat_a.c
gcc -o fifo_chat_b fifo_chat_b.c

# 在两个终端分别运行
rm /tmp/fifo_* 2>/dev/null
# 终端 1：./fifo_chat_a
# 终端 2：./fifo_chat_b
```
