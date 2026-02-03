# 目录遍历模式与工具函数

本章介绍目录遍历常见写法与相关 API：`readdir` 循环、`scandir` 过滤排序，以及递归遍历注意事项。

## 1. 基本遍历模板（readdir 循环）

步骤：

1. `opendir(path)`
2. 循环 `readdir(dp)`
3. 跳过 `.` 与 `..`
4. 拼接子路径并做 `lstat`/递归
5. `closedir(dp)`

拼接路径的建议：

- 注意 `path` 是否以 `/` 结尾
- 使用 `snprintf` 防止缓冲区溢出

## 2. scandir：一次性读取并可排序

```c
#include <dirent.h>

int scandir(const char *dirp, struct dirent ***namelist,
			int (*filter)(const struct dirent *),
			int (*compar)(const struct dirent **, const struct dirent **));
```

常见用途：

- 过滤隐藏文件
- 按字典序排序（`alphasort`）

注意：`scandir` 会分配内存，需要逐个 `free(namelist[i])` 再 `free(namelist)`。

## 3. 递归遍历注意事项

- **符号链接**：递归时要决定是否跟随；跟随可能导致环。
- **权限不足**：遇到 `EACCES` 时跳过并继续。
- **深度限制**：避免栈溢出（可做深度限制或改显式栈）。

## 4. 练习

1. 实现简化版 `tree`：递归打印目录结构。
2. 实现简化版 `du`：统计目录总大小（注意硬链接与重复计算的问题可先忽略）。

### 参考实现

**练习 1: 简化版 tree**

```c
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_tree(const char *path, const char *prefix, int depth) {
    if (depth > 10) {  // 防止太深的递归
        return;
    }

    DIR *dp = opendir(path);
    if (!dp) {
        perror(path);
        return;
    }

    struct dirent *ent;
    int count = 0;
    
    while ((ent = readdir(dp)) != NULL) {
        // 跳过 . 和 ..
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }
        count++;
    }
    rewinddir(dp);

    int current = 0;
    while ((ent = readdir(dp)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        current++;
        int is_last = (current == count);
        
        // 打印树形符号
        printf("%s%s", prefix, is_last ? "└── " : "├── ");
        printf("%s\n", ent->d_name);

        // 构造子路径
        char subpath[4096];
        snprintf(subpath, sizeof(subpath), "%s/%s", path, ent->d_name);

        // 检查是否为目录
        struct stat st;
        if (lstat(subpath, &st) == 0 && S_ISDIR(st.st_mode)) {
            // 递归处理子目录
            char new_prefix[4096];
            snprintf(new_prefix, sizeof(new_prefix), "%s%s",
                     prefix, is_last ? "    " : "│   ");
            print_tree(subpath, new_prefix, depth + 1);
        }
    }

    closedir(dp);
}

int main(int argc, char **argv) {
    const char *path = argc > 1 ? argv[1] : ".";
    
    printf("%s\n", path);
    print_tree(path, "", 0);
    
    return 0;
}
```

编译运行：
```bash
gcc -o simple_tree simple_tree.c
./simple_tree /etc
./simple_tree .
```

**练习 2: 简化版 du（磁盘占用）**

```c
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 返回目录总大小（字节）
long get_dir_size(const char *path) {
    DIR *dp = opendir(path);
    if (!dp) {
        return 0;
    }

    long total = 0;
    struct dirent *ent;
    
    while ((ent = readdir(dp)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        char subpath[4096];
        snprintf(subpath, sizeof(subpath), "%s/%s", path, ent->d_name);

        struct stat st;
        if (lstat(subpath, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                // 递归计算子目录
                total += get_dir_size(subpath);
            } else {
                // 累加文件大小
                total += st.st_size;
            }
        }
    }

    closedir(dp);
    return total;
}

void print_dir_size(const char *path, int depth) {
    if (depth > 5) return;

    DIR *dp = opendir(path);
    if (!dp) {
        return;
    }

    struct dirent *ent;
    struct {
        char name[256];
        long size;
    } *dirs = NULL;
    int dir_count = 0;
    int dir_capacity = 0;

    // 收集所有子目录和大小
    while ((ent = readdir(dp)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        char subpath[4096];
        snprintf(subpath, sizeof(subpath), "%s/%s", path, ent->d_name);

        struct stat st;
        if (lstat(subpath, &st) == 0 && S_ISDIR(st.st_mode)) {
            if (dir_count >= dir_capacity) {
                dir_capacity = (dir_capacity == 0) ? 10 : dir_capacity * 2;
                dirs = realloc(dirs, sizeof(*dirs) * dir_capacity);
            }
            
            strcpy(dirs[dir_count].name, ent->d_name);
            dirs[dir_count].size = get_dir_size(subpath);
            dir_count++;
        }
    }

    // 按大小排序（降序）
    for (int i = 0; i < dir_count - 1; i++) {
        for (int j = i + 1; j < dir_count; j++) {
            if (dirs[j].size > dirs[i].size) {
                struct {char name[256]; long size;} tmp = dirs[i];
                dirs[i] = dirs[j];
                dirs[j] = tmp;
            }
        }
    }

    // 打印结果
    for (int i = 0; i < dir_count; i++) {
        printf("%8ld KB  %s/%s\n", dirs[i].size / 1024, path, dirs[i].name);
    }

    free(dirs);
    closedir(dp);
}

int main(int argc, char **argv) {
    const char *path = argc > 1 ? argv[1] : ".";
    
    long total = get_dir_size(path);
    printf("Total: %ld KB\n\n", total / 1024);
    
    print_dir_size(path, 0);
    
    return 0;
}
```

编译运行：
```bash
gcc -o simple_du simple_du.c
./simple_du /home
./simple_du .
```

**关键点：**
- `tree` 需要处理目录遍历和树形输出格式
- `du` 需要递归计算大小
- 都需要正确处理 `.` 和 `..` 的跳过
- 处理权限错误时应继续处理其他项

