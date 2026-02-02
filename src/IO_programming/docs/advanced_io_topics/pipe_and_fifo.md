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
