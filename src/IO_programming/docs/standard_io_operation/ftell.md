# ftell：获取流当前位置

`ftell(3)` 返回 `FILE*` 流的当前位置（偏移量）。

## 1. 头文件与函数原型

```c
#include <stdio.h>

long ftell(FILE *stream);
```

## 2. 返回值

- 成功：返回非负 `long`
- 失败：返回 `-1L`，并设置 `errno`

## 3. 注意

当流有缓冲时，`ftell` 返回的是逻辑位置（已考虑缓冲），一般用于配合 `fseek` 回退。

## 4. 练习

写程序：在读取过程中记录 `ftell`，然后 `fseek` 回到记录的位置并再次读取，验证一致性。

