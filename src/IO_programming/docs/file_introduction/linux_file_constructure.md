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

