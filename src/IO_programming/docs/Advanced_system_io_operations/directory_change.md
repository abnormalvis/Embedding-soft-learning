# chdir/getcwd：切换与获取当前工作目录

当前工作目录（CWD）影响相对路径解析，是很多工具程序的基础概念。

## 1. 头文件与函数原型

```c
#include <unistd.h>

int chdir(const char *path);
char *getcwd(char *buf, size_t size);
```

## 2. 返回值

- `chdir`：成功 `0`，失败 `-1` + `errno`
- `getcwd`：成功返回 `buf`，失败返回 `NULL` + `errno`

## 3. getcwd 的用法

```c
char cwd[4096];
if (!getcwd(cwd, sizeof(cwd))) {
	perror("getcwd");
	return 1;
}
printf("cwd=%s\n", cwd);
```

如果 `buf == NULL`，glibc 允许由 `getcwd` 分配（需 `free`），但为可移植性通常建议显式提供缓冲。

## 4. 常见 errno

- `ENOENT` 路径不存在
- `ENOTDIR` 不是目录
- `EACCES` 权限不足

## 5. 练习

实现：`cd-print` 程序，参数是目录路径，`chdir` 后打印新的 CWD。

