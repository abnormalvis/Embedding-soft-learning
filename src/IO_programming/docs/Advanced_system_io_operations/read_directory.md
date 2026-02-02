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

