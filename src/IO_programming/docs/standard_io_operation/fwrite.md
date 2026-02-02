# fwrite：块写入

`fwrite(3)` 向 `FILE*` 流写入“元素数组”。注意它写到的是 stdio 缓冲区，不一定立刻落到内核/磁盘。

## 1. 头文件与函数原型

```c
#include <stdio.h>

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
```

## 2. 返回值

返回“成功写入的元素个数”。如果小于 `nmemb`，通常表示错误。

```c
size_t n = fwrite(buf, 1, len, fp);
if (n != len) {
	perror("fwrite");
}
```

## 3. 刷新时机

输出缓冲通常在以下时刻刷出：

- 缓冲满
- 遇到换行（行缓冲流，如终端上的 stdout）
- 显式调用 `fflush(fp)`
- 调用 `fclose(fp)`

## 4. 练习

写程序：`fwrite` 后不 `fflush` 直接 `sleep(10)`，另一个终端观察文件内容何时出现。

