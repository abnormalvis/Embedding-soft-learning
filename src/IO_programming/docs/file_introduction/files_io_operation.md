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

