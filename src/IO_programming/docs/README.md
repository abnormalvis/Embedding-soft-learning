# IO_programming 文档索引

这组文档面向 Linux 下的 IO 编程学习，覆盖：系统调用 IO（FD）、C 标准 IO（FILE*）、以及目录相关接口。

## 1. 入门：概念与地图

- [Linux 文件结构与文件描述符](file_introduction/linux_file_constructure.md)
- [系统调用入门](file_introduction/system_call.md)
- [C 函数库与 IO：libc 视角](file_introduction/function_library.md)
- [Linux 文件 IO 操作总览](file_introduction/files_io_operation.md)

## 2. 系统调用 IO（POSIX）

- [open：打开/创建文件](system_io_operation/open.md)
- [close：关闭文件描述符](system_io_operation/close.md)
- [read：读取数据](system_io_operation/read.md)
- [write：写入数据](system_io_operation/write.md)
- [lseek：移动文件偏移](system_io_operation/lseek.md)
- [stat：获取文件元信息](system_io_operation/stat.md)
- [access：以真实身份检查权限](system_io_operation/access.md)

## 3. 标准 IO（stdio）

- [fopen：打开文件（stdio）](standard_io_operation/fopen.md)
- [fclose：关闭流并刷新缓冲](standard_io_operation/fclose.md)
- [fread：块读取](standard_io_operation/fread.md)
- [fwrite：块写入](standard_io_operation/fwrite.md)
- [fflush：刷新输出缓冲](standard_io_operation/fflush.md)
- [fseek：移动流位置](standard_io_operation/fseek.md)
- [ftell：获取流当前位置](standard_io_operation/ftell.md)
- [探索 C 标准库缓冲](standard_io_operation/exploring_c_library_buffer.md)

## 4. 目录相关（dirent）

- [opendir/closedir：打开与关闭目录流](Advanced_system_io_operations/open_and_close_directories.md)
- [readdir：读取目录项](Advanced_system_io_operations/read_directory.md)
- [目录遍历模式与工具函数](Advanced_system_io_operations/directory_traversal.md)
- [chdir/getcwd：切换与获取当前工作目录](Advanced_system_io_operations/directory_change.md)
- [mkdir/rmdir：创建与删除目录](Advanced_system_io_operations/create_and_delete_directories.md)

## 5. 高级 IO 主题

- [fcntl：非阻塞、CLOEXEC、文件锁](advanced_io_topics/fcntl.md)
- [dup/dup2/dup3：复制 FD 与重定向](advanced_io_topics/dup_dup2.md)
- [pread/pwrite：原子随机读写](advanced_io_topics/pread_pwrite.md)
- [mmap：内存映射文件](advanced_io_topics/mmap.md)
- [pipe/FIFO：管道与命名管道](advanced_io_topics/pipe_and_fifo.md)
- [select/poll/epoll：IO 多路复用概览](advanced_io_topics/select_poll_epoll.md)
- [信号与 IO：EINTR 与 SA_RESTART](advanced_io_topics/signals_and_eintr.md)

## 6. 示例代码如何运行

在 [src/IO_programming](../) 目录下：

- 构建全部示例：`make`
- 清理：`make clean`
- 运行示例：`./build/bin/<name> [args...]`

示例：

- `./build/bin/cat_fd /etc/hosts`
- `./build/bin/dup_redirect out.txt`
- `./build/bin/pipe_demo`


## 7. 下一步建议

如果你希望继续扩展本目录的 IO 学习内容，推荐补充以下主题（可新建文档或在现有文档追加章节）：

- socket IO：`connect/accept/send/recv`、半关闭、粘包概念
- 零拷贝：`sendfile/splice`（Linux）
- 日志与文件滚动：原子 rename、fsync、崩溃一致性
