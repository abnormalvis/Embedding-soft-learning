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

### 参考实现

**练习 1: mkdirp - 逐级创建目录**

```c
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int mkdirp(const char *path, mode_t mode) {
    if (strlen(path) == 0) return 0;

    // 创建路径的副本用于修改
    char *path_copy = strdup(path);
    if (!path_copy) {
        perror("strdup");
        return -1;
    }

    // 从第一个 / 之后开始逐个创建目录
    for (char *p = path_copy + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';  // 临时截断
            
            // 尝试创建目录
            if (mkdir(path_copy, mode) == -1) {
                if (errno != EEXIST) {
                    perror(path_copy);
                    free(path_copy);
                    return -1;
                }
                // EEXIST 是可以接受的
            } else {
                printf("Created: %s\n", path_copy);
            }
            
            *p = '/';  // 恢复 /
        }
    }

    // 创建最后一级目录
    if (mkdir(path_copy, mode) == -1) {
        if (errno != EEXIST) {
            perror(path_copy);
            free(path_copy);
            return -1;
        }
    } else {
        printf("Created: %s\n", path_copy);
    }

    free(path_copy);
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <path> [path2] ...\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        printf("Creating: %s\n", argv[i]);
        if (mkdirp(argv[i], 0755) == -1) {
            fprintf(stderr, "Failed to create %s\n", argv[i]);
            return 1;
        }
    }

    return 0;
}
```

编译运行：
```bash
gcc -o mkdirp mkdirp.c
./mkdirp /tmp/a/b/c/d
./mkdirp /tmp/test/x /tmp/test/y /tmp/test/z
ls -la /tmp/a/b/c/
```

**练习 2: rmtree - 安全的递归删除**

```c
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char **paths;
    int count;
    int capacity;
} PathList;

void path_list_add(PathList *pl, const char *path) {
    if (pl->count >= pl->capacity) {
        pl->capacity = (pl->capacity == 0) ? 10 : pl->capacity * 2;
        pl->paths = realloc(pl->paths, sizeof(char*) * pl->capacity);
    }
    pl->paths[pl->count++] = strdup(path);
}

int plan_removal(const char *path, PathList *plan) {
    struct stat st;
    if (lstat(path, &st) == -1) {
        perror(path);
        return 0;
    }

    if (S_ISDIR(st.st_mode)) {
        DIR *dp = opendir(path);
        if (!dp) {
            perror(path);
            return 0;
        }

        struct dirent *ent;
        while ((ent = readdir(dp)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                continue;
            }

            char subpath[4096];
            snprintf(subpath, sizeof(subpath), "%s/%s", path, ent->d_name);
            plan_removal(subpath, plan);
        }
        closedir(dp);
        
        // 目录本身放在最后（先删内容再删目录）
        path_list_add(plan, path);
    } else {
        path_list_add(plan, path);
    }

    return 1;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <path> [path2] ...\n", argv[0]);
        fprintf(stderr, "WARNING: This will DELETE the directory and all contents!\n");
        return 1;
    }

    PathList plan = {0};

    // 第一步：规划要删除的所有内容
    printf("=== Removal Plan ===\n");
    for (int i = 1; i < argc; i++) {
        plan_removal(argv[i], &plan);
    }

    printf("Total items to remove: %d\n\n", plan.count);
    printf("Items (in order):\n");
    for (int i = 0; i < plan.count; i++) {
        printf("  %s\n", plan.paths[i]);
    }

    // 确认删除
    printf("\nProceed with deletion? (yes/no): ");
    char response[10];
    fgets(response, sizeof(response), stdin);
    
    if (strcmp(response, "yes\n") != 0) {
        printf("Cancelled.\n");
        return 0;
    }

    // 第二步：执行删除
    printf("\n=== Executing Removal ===\n");
    int removed = 0;
    for (int i = 0; i < plan.count; i++) {
        if (rmdir(plan.paths[i]) == 0) {
            printf("Removed dir:  %s\n", plan.paths[i]);
            removed++;
        } else if (unlink(plan.paths[i]) == 0) {
            printf("Removed file: %s\n", plan.paths[i]);
            removed++;
        } else {
            perror(plan.paths[i]);
        }
    }

    printf("\nSuccessfully removed: %d items\n", removed);

    // 清理
    for (int i = 0; i < plan.count; i++) {
        free(plan.paths[i]);
    }
    free(plan.paths);

    return 0;
}
```

编译运行：
```bash
gcc -o rmtree rmtree.c
mkdir -p /tmp/test_rm/subdir/deep
touch /tmp/test_rm/file1.txt /tmp/test_rm/subdir/file2.txt
./rmtree /tmp/test_rm
# 然后输入 yes 确认删除
```

**关键点：**

**mkdirp：**
- 逐级创建，检查 `EEXIST` 错误（已存在不算错误）
- 需要处理路径中的多个 `/`
- 使用 `mode_t` 指定权限

**rmtree：**
- 先规划再执行，给用户确认机会
- 递归删除前先删除文件和空子目录
- 使用 `unlink()` 删除文件，`rmdir()` 删除空目录
- 通常不跟随符号链接（使用 `lstat()` 而不是 `stat()`）

