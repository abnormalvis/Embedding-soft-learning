# stat：获取文件元信息

`stat(2)` 获取文件的元信息（类型、权限、大小、时间戳等）。

## 1. 头文件与函数原型

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int stat(const char *pathname, struct stat *statbuf);
int fstat(int fd, struct stat *statbuf);
int lstat(const char *pathname, struct stat *statbuf);
```

区别：

- `stat`：跟随符号链接
- `lstat`：不跟随符号链接
- `fstat`：对已打开的 FD 获取信息

## 2. struct stat 常用字段

- `st_mode`：文件类型 + 权限位
- `st_size`：文件大小（字节）
- `st_uid`/`st_gid`：所有者
- `st_atime`/`st_mtime`/`st_ctime`：访问/修改/状态变更时间

判断文件类型：

```c
if (S_ISREG(st.st_mode)) { /* regular file */ }
if (S_ISDIR(st.st_mode)) { /* directory */ }
```

## 3. 常见 errno

- `ENOENT` 路径不存在
- `EACCES` 权限不足
- `ENOTDIR` 路径中某一段不是目录

## 4. 练习

实现 `ls -l` 的一个子集：打印文件类型（- d l p s 等）、权限字符串与 `st_size`。

