# mkdir/rmdir：创建与删除目录

创建目录使用 `mkdir(2)`，删除空目录使用 `rmdir(2)`。

## 1. 头文件与函数原型

```c
#include <sys/stat.h>
#include <sys/types.h>

int mkdir(const char *pathname, mode_t mode);

#include <unistd.h>

int rmdir(const char *pathname);
```

## 2. mkdir：权限与 umask

`mode` 会被 `umask` 掩码影响（参考“文件结构”章节）。常见：

```c
if (mkdir("demo", 0755) == -1) {
	perror("mkdir");
}
```

## 3. rmdir：只能删除空目录

```c
if (rmdir("demo") == -1) {
	perror("rmdir");
}
```

目录非空会报 `ENOTEMPTY` 或 `EEXIST`（与实现相关）。

## 4. 删除非空目录怎么办

需要递归删除目录内容：

- 先遍历目录项（`opendir/readdir`）
- 对普通文件用 `unlink()`
- 对子目录递归后再 `rmdir()`

这是实现 `rm -r` 的核心。

## 5. 练习

1. 写 `mkdirp`：实现 `mkdir -p` 的行为（逐级创建）。
2. 写 `rmtree`：实现安全的递归删除（先打印计划删除的内容，再执行）。

