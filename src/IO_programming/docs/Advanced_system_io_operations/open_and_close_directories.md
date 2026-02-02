# opendir/closedir：打开与关闭目录流

目录遍历通常使用 `dirent.h` 提供的“目录流”（`DIR*`），而不是把目录当作普通文件用 `open/read` 读取。

## 1. 头文件与函数原型

```c
#include <dirent.h>

DIR *opendir(const char *name);
int closedir(DIR *dirp);
```

## 2. 返回值与错误

- `opendir` 成功返回非空 `DIR*`，失败返回 `NULL` 并设置 `errno`
- `closedir` 成功返回 `0`，失败返回 `-1`

常见 errno：`ENOENT`、`EACCES`、`ENOTDIR`。

## 3. 示例：打开目录并关闭

```c
DIR *dp = opendir(".");
if (!dp) {
	perror("opendir");
	return 1;
}

if (closedir(dp) == -1) {
	perror("closedir");
	return 1;
}
```

## 4. 补充：fdopendir

有时你手里已有目录 FD（如 `open(".", O_RDONLY|O_DIRECTORY)`），可以：

```c
#include <fcntl.h>
#include <dirent.h>

DIR *fdopendir(int fd);
```

注意：`closedir` 会关闭底层 FD。

