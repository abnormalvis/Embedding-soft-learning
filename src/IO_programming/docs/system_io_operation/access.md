# access：以真实身份检查权限

`access(2)` 检查调用进程“以真实 UID/GID”对文件的可访问性。

## 1. 头文件与函数原型

```c
#include <unistd.h>

int access(const char *pathname, int mode);
```

`mode` 可取：

- `F_OK`：文件是否存在
- `R_OK`：是否可读
- `W_OK`：是否可写
- `X_OK`：是否可执行

## 2. 返回值

- 成功：`0`
- 失败：`-1` 并设置 `errno`

## 3. 重要警告：TOCTOU

经典坑：

```c
if (access(path, W_OK) == 0) {
	fd = open(path, O_WRONLY);
}
```

在 `access` 和 `open` 之间，文件可能被替换/权限可能变化，导致安全问题（Time Of Check To Time Of Use）。

更可靠的做法是：

- 直接 `open()`，根据返回的 `errno` 判断
- 或在需要安全保证时使用更严谨的策略（如目录 FD + `openat`、避免跟随符号链接等）

## 4. 常见 errno

- `ENOENT` 不存在
- `EACCES` 权限不足
- `EROFS` 只读文件系统（对写检查）

## 5. 练习

写程序：输入路径，分别用 `access(..., F_OK/R_OK/W_OK/X_OK)` 打印可访问性，并解释和 `stat` 权限位之间的差异。

### 参考实现

```c
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return 1;
    }

    const char *path = argv[1];

    // 检查可访问性
    printf("Access check for: %s\n", path);
    printf("  F_OK (exist):   %s\n", access(path, F_OK) == 0 ? "yes" : "no");
    printf("  R_OK (read):    %s\n", access(path, R_OK) == 0 ? "yes" : "no");
    printf("  W_OK (write):   %s\n", access(path, W_OK) == 0 ? "yes" : "no");
    printf("  X_OK (execute): %s\n", access(path, X_OK) == 0 ? "yes" : "no");

    // 对比 stat 权限位
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("stat");
        return 1;
    }

    printf("\nstat mode: %o\n", st.st_mode & 0777);
    printf("  Owner: r=%d w=%d x=%d\n",
           (st.st_mode & 0400) ? 1 : 0,
           (st.st_mode & 0200) ? 1 : 0,
           (st.st_mode & 0100) ? 1 : 0);
    printf("  Group: r=%d w=%d x=%d\n",
           (st.st_mode & 0040) ? 1 : 0,
           (st.st_mode & 0020) ? 1 : 0,
           (st.st_mode & 0010) ? 1 : 0);
    printf("  Other: r=%d w=%d x=%d\n",
           (st.st_mode & 0004) ? 1 : 0,
           (st.st_mode & 0002) ? 1 : 0,
           (st.st_mode & 0001) ? 1 : 0);

    printf("\nNote: access() checks by real UID/GID, stat() just shows raw bits.\n");
    return 0;
}
```

编译运行：
```bash
gcc -o check_access check_access.c
./check_access /etc/passwd
```

