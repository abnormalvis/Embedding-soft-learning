# fflush：刷新输出缓冲

`fflush(3)` 把 stdio 的输出缓冲刷新到底层（通常对应 `write(2)`）。

## 1. 头文件与函数原型

```c
#include <stdio.h>

int fflush(FILE *stream);
```

## 2. 参数与返回值

- `stream != NULL`：刷新指定流
- `stream == NULL`：刷新所有打开的输出流（常用于程序退出前）

返回值：

- 成功：`0`
- 失败：`EOF`，设置 `errno`

## 3. 典型使用场景

- 交互式程序提示：`printf("Enter..."); fflush(stdout);`
- `fork()` 前：避免父进程缓冲内容在父子进程中各自刷一次造成重复输出

## 4. 注意

`fflush` 对“输入流”在标准上是未定义/实现相关行为（POSIX 有部分扩展语义），实际开发中避免对纯输入流使用。

