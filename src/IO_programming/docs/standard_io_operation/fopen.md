# fopen：打开文件（stdio）

`fopen(3)` 打开文件并返回 `FILE*` 流。它是对底层 `open(2)` 的封装，并在用户态维护缓冲。

## 1. 头文件与函数原型

```c
#include <stdio.h>

FILE *fopen(const char *pathname, const char *mode);
```

## 2. mode 字符串

常见模式：

- `"r"`：只读（文件必须存在）
- `"w"`：只写（不存在则创建；存在则截断）
- `"a"`：追加写（不存在则创建）
- `"r+"`：读写（必须存在）
- `"w+"`：读写（创建或截断）
- `"a+"`：读写追加

可选附加：

- `"b"`：二进制模式（在 Linux 上通常无区别，但为跨平台可保留）
- `"e"`：设置 `O_CLOEXEC`（glibc 扩展，非标准）

## 3. 返回值与错误

- 成功：返回非空 `FILE*`
- 失败：返回 `NULL`，并设置 `errno`

打印错误：

```c
FILE *fp = fopen("a.txt", "r");
if (!fp) {
	perror("fopen");
}
```

## 4. 示例：逐行读取

```c
char line[1024];
while (fgets(line, sizeof(line), fp) != NULL) {
	fputs(line, stdout);
}
```

## 5. 常见坑

- 混用 `FILE*` 和 FD：如果你用 `fopen` 打开，原则上用 `fclose` 关闭。
- 忘记处理输出缓冲：程序异常退出或 `fork` 后，缓冲可能重复输出或丢失。

