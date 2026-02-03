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

### 参考实现

```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        return 1;
    }

    const char *target_dir = argv[1];

    // 保存原有 CWD
    char old_cwd[4096];
    if (getcwd(old_cwd, sizeof(old_cwd)) == NULL) {
        perror("getcwd (old)");
        return 1;
    }
    printf("Original CWD: %s\n", old_cwd);

    // 尝试改变工作目录
    if (chdir(target_dir) == -1) {
        perror("chdir");
        return 1;
    }
    printf("Changed directory to: %s\n", target_dir);

    // 获取新的 CWD
    char new_cwd[4096];
    if (getcwd(new_cwd, sizeof(new_cwd)) == NULL) {
        perror("getcwd (new)");
        return 1;
    }
    printf("New CWD: %s\n", new_cwd);

    // 列出当前目录的文件
    printf("\nFiles in %s:\n", new_cwd);
    system("ls -la");

    return 0;
}
```

编译运行：
```bash
gcc -o cd_print cd_print.c
./cd_print /tmp
./cd_print /etc
./cd_print .
```

**高级版本：比较相对和绝对路径**

```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

void print_path_info(const char *path) {
    printf("\n=== Directory: %s ===\n", path);
    
    // 保存当前工作目录
    char saved_cwd[4096];
    getcwd(saved_cwd, sizeof(saved_cwd));

    // 进入目录
    if (chdir(path) == -1) {
        perror("chdir");
        return;
    }

    // 获取工作目录
    char cwd[4096];
    getcwd(cwd, sizeof(cwd));
    printf("CWD: %s\n", cwd);

    // 显示父目录
    char parent[4096];
    strcpy(parent, cwd);
    printf("Parent: %s\n", dirname(parent));

    // 回到原来的目录
    chdir(saved_cwd);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <path1> [path2] ...\n", argv[0]);
        return 1;
    }

    printf("Initial CWD: %s\n", getcwd(NULL, 0));

    for (int i = 1; i < argc; i++) {
        print_path_info(argv[i]);
    }

    char final_cwd[4096];
    getcwd(final_cwd, sizeof(final_cwd));
    printf("\n\nFinal CWD: %s\n", final_cwd);

    return 0;
}
```

编译运行：
```bash
gcc -o cd_explorer cd_explorer.c
./cd_explorer /tmp /etc /home
```

**关键点：**
- `chdir()` 改变进程的工作目录（只影响该进程及其子进程）
- `getcwd()` 获取当前工作目录（需要足够大的缓冲区）
- 对于相对路径 `chdir()` 会相对于当前 CWD
- 进程退出后工作目录不会对父进程有影响
- 如果 `getcwd()` 的 `buf` 为 NULL，会动态分配（需要 `free()`）

