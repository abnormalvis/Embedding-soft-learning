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

### 参考实现

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char get_file_type(mode_t mode) {
    if (S_ISREG(mode)) return '-';
    if (S_ISDIR(mode)) return 'd';
    if (S_ISLNK(mode)) return 'l';
    if (S_ISCHR(mode)) return 'c';
    if (S_ISBLK(mode)) return 'b';
    if (S_ISFIFO(mode)) return 'p';
    if (S_ISSOCK(mode)) return 's';
    return '?';
}

void print_permissions(mode_t mode) {
    printf("%c", get_file_type(mode));
    
    // Owner permissions
    printf("%c%c%c",
           (mode & S_IRUSR) ? 'r' : '-',
           (mode & S_IWUSR) ? 'w' : '-',
           (mode & S_IXUSR) ? 'x' : '-');
    
    // Group permissions
    printf("%c%c%c",
           (mode & S_IRGRP) ? 'r' : '-',
           (mode & S_IWGRP) ? 'w' : '-',
           (mode & S_IXGRP) ? 'x' : '-');
    
    // Other permissions
    printf("%c%c%c",
           (mode & S_IROTH) ? 'r' : '-',
           (mode & S_IWOTH) ? 'w' : '-',
           (mode & S_IXOTH) ? 'x' : '-');
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> [file2] ...\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        const char *path = argv[i];
        struct stat st;

        // 使用 lstat 以避免跟随符号链接
        if (lstat(path, &st) == -1) {
            perror(path);
            continue;
        }

        // 打印权限
        print_permissions(st.st_mode);

        // 打印大小
        printf(" %10ld", (long)st.st_size);

        // 打印文件名
        printf(" %s\n", path);
    }

    return 0;
}
```

编译运行：
```bash
gcc -o ls_mini ls_mini.c
./ls_mini /etc/passwd /etc /etc/shadow 2>/dev/null
```

**预期输出类似：**
```
---------- 4294967295 /etc/passwd
drwxr-xr-x       4096 /etc
-rw-r----- 0 /etc/shadow (permission denied)
```

**关键点：**
- `S_ISREG()` 等宏检查文件类型
- 通过位操作检查各类权限位
- 使用 `lstat()` 而不是 `stat()` 以正确处理符号链接
- `-10ld` 格式右对齐大小字段，便于列表阅读

