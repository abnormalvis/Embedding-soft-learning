# readdir：读取目录项

`readdir(3)` 从目录流中读取下一个目录项（`struct dirent`）。

## 1. 头文件与函数原型

```c
#include <dirent.h>

struct dirent *readdir(DIR *dirp);
```

## 2. 返回值语义

- 成功：返回指向内部静态区的 `struct dirent*`
- 到末尾：返回 `NULL`，且 `errno` 不变
- 出错：返回 `NULL`，并设置 `errno`

因此典型写法：

```c
errno = 0;
struct dirent *ent = readdir(dp);
if (!ent) {
	if (errno != 0) perror("readdir");
	// 否则是正常结束
}
```

## 3. struct dirent 常用字段

- `d_name`：条目名（不包含路径）
- `d_ino`：inode（不总是有意义，取决于文件系统）
- `d_type`：类型（`DT_REG`/`DT_DIR`/...，可能是 `DT_UNKNOWN`）

注意：不要完全依赖 `d_type`，更稳妥是对条目路径做 `lstat()`。

## 4. 示例：列出目录下的文件名

```c
DIR *dp = opendir(path);
if (!dp) { perror("opendir"); return 1; }

for (;;) {
	errno = 0;
	struct dirent *ent = readdir(dp);
	if (!ent) {
		if (errno) perror("readdir");
		break;
	}
	if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
		continue;
	printf("%s\n", ent->d_name);
}
closedir(dp);
```

## 5. 练习

把上面的程序改成“递归遍历”，并对每个条目打印类型与大小（提示：拼接路径 + `lstat`）。
### 参考实现

```c
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void recursive_list(const char *path, int depth) {
    DIR *dp = opendir(path);
    if (!dp) {
        perror(path);
        return;
    }

    struct dirent *ent;
    errno = 0;
    
    while ((ent = readdir(dp)) != NULL) {
        // 跳过 . 和 ..
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        // 拼接完整路径
        char fullpath[4096];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, ent->d_name);

        // 获取文件信息（使用 lstat 避免跟随符号链接）
        struct stat st;
        if (lstat(fullpath, &st) == -1) {
            perror(fullpath);
            continue;
        }

        // 打印缩进
        for (int i = 0; i < depth; i++) {
            printf("  ");
        }

        // 打印类型
        char type;
        if (S_ISREG(st.st_mode)) {
            type = '-';
        } else if (S_ISDIR(st.st_mode)) {
            type = 'd';
        } else if (S_ISLNK(st.st_mode)) {
            type = 'l';
        } else if (S_ISCHR(st.st_mode)) {
            type = 'c';
        } else if (S_ISBLK(st.st_mode)) {
            type = 'b';
        } else if (S_ISFIFO(st.st_mode)) {
            type = 'p';
        } else if (S_ISSOCK(st.st_mode)) {
            type = 's';
        } else {
            type = '?';
        }

        // 打印信息：类型、大小、名称
        printf("%c %10ld %s\n", type, (long)st.st_size, ent->d_name);

        // 如果是目录，递归处理
        if (S_ISDIR(st.st_mode)) {
            recursive_list(fullpath, depth + 1);
        }
    }

    // 检查读取错误
    if (errno != 0) {
        perror("readdir");
    }

    closedir(dp);
}

int main(int argc, char **argv) {
    const char *path = argc > 1 ? argv[1] : ".";
    
    printf("Recursive listing of: %s\n\n", path);
    recursive_list(path, 0);
    
    return 0;
}
```

编译运行：
```bash
gcc -o recursive_list recursive_list.c
./recursive_list /etc/ssl
./recursive_list .
```

**改进版本：更详细的信息和颜色输出**

```c
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define RESET   "\x1b[0m"
#define BOLD    "\x1b[1m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define BLUE    "\x1b[34m"

const char *get_type_name(mode_t mode) {
    if (S_ISREG(mode)) return "file";
    if (S_ISDIR(mode)) return "dir";
    if (S_ISLNK(mode)) return "link";
    if (S_ISCHR(mode)) return "chr";
    if (S_ISBLK(mode)) return "blk";
    if (S_ISFIFO(mode)) return "fifo";
    if (S_ISSOCK(mode)) return "sock";
    return "?";
}

void detailed_list(const char *path, int depth) {
    DIR *dp = opendir(path);
    if (!dp) {
        perror(path);
        return;
    }

    struct dirent *ent;
    
    while ((ent = readdir(dp)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        char fullpath[4096];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, ent->d_name);

        struct stat st;
        if (lstat(fullpath, &st) == -1) {
            perror(fullpath);
            continue;
        }

        // 打印缩进
        for (int i = 0; i < depth; i++) {
            printf("  ");
        }

        // 类型
        const char *type = get_type_name(st.st_mode);
        
        // 权限
        char perms[10];
        snprintf(perms, sizeof(perms), "%c%c%c%c%c%c%c%c%c",
                 (st.st_mode & 0400) ? 'r' : '-',
                 (st.st_mode & 0200) ? 'w' : '-',
                 (st.st_mode & 0100) ? 'x' : '-',
                 (st.st_mode & 040) ? 'r' : '-',
                 (st.st_mode & 020) ? 'w' : '-',
                 (st.st_mode & 010) ? 'x' : '-',
                 (st.st_mode & 04) ? 'r' : '-',
                 (st.st_mode & 02) ? 'w' : '-',
                 (st.st_mode & 01) ? 'x' : '-');

        // 时间戳
        char timestr[32];
        time_t mtime = st.st_mtime;
        strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M", localtime(&mtime));

        // 颜色输出
        if (S_ISDIR(st.st_mode)) {
            printf("%s%-5s %s %10ld %s %s/%s\n", BOLD BLUE, type, perms, 
                   (long)st.st_size, timestr, path, ent->d_name RESET);
        } else if (S_ISLNK(st.st_mode)) {
            printf("%s%-5s %s %10ld %s %s\n", BOLD, type, perms, 
                   (long)st.st_size, timestr, ent->d_name RESET);
        } else {
            printf("%-5s %s %10ld %s %s\n", type, perms, 
                   (long)st.st_size, timestr, ent->d_name);
        }

        // 递归处理子目录
        if (S_ISDIR(st.st_mode)) {
            detailed_list(fullpath, depth + 1);
        }
    }

    closedir(dp);
}

int main(int argc, char **argv) {
    const char *path = argc > 1 ? argv[1] : ".";
    detailed_list(path, 0);
    return 0;
}
```

编译运行：
```bash
gcc -o detailed_list detailed_list.c
./detailed_list /tmp
```

**关键点：**
- 必须使用 `lstat()` 而不是 `stat()` 以正确处理符号链接
- 完整路径拼接需要注意缓冲区溢出
- 递归应有深度限制防止无限循环
- 错误处理要持续，一个项目的错误不应中止整个遍历
