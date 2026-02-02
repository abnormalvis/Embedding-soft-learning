# fseek：移动流位置

`fseek(3)` 类似 `lseek(2)`，但操作对象是 `FILE*`。

## 1. 头文件与函数原型

```c
#include <stdio.h>

int fseek(FILE *stream, long offset, int whence);
```

`whence`：`SEEK_SET`/`SEEK_CUR`/`SEEK_END`。

## 2. 返回值

- 成功：`0`
- 失败：非 0，并设置 `errno`

## 3. 注意事项

- 文本模式下的随机访问在某些平台可能受换行转换影响；Linux 上通常问题不大，但跨平台请谨慎。
- 先写后读/先读后写：在同一 `FILE*` 上切换方向前，通常需要 `fflush` 或 `fseek`/`fsetpos` 来同步缓冲状态。

## 4. 练习

写程序：读取一个文件末尾 100 字节（先 `fseek(fp, -100, SEEK_END)` 再 `fread`）。

