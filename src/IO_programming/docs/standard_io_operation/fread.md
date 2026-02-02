# fread：块读取

`fread(3)` 从 `FILE*` 流读取“元素数组”，常用于二进制结构或块式读取。

## 1. 头文件与函数原型

```c
#include <stdio.h>

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
```

它尝试读取 `nmemb` 个元素，每个元素 `size` 字节。

## 2. 返回值语义

返回“成功读取的元素个数”，范围 `0..nmemb`。

常见判定：

- 返回 `nmemb`：完整读到
- 返回 `< nmemb`：可能遇到 EOF 或错误

错误与 EOF 的区分：

```c
if (n < nmemb) {
	if (ferror(fp)) {
		perror("fread");
	}
	if (feof(fp)) {
		// EOF
	}
}
```

## 3. 示例：按块拷贝文件

```c
char buf[4096];
size_t n;
while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
	if (fwrite(buf, 1, n, out) != n) {
		perror("fwrite");
		break;
	}
}
if (ferror(in)) perror("fread");
```

