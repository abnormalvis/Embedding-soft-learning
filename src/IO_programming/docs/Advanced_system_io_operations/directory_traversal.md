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

