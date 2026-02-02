# lseek：移动文件偏移

`lseek(2)` 用于设置/查询“打开文件描述（open file description）”中的当前偏移量，主要用于常规文件的随机访问。

## 1. 头文件与函数原型

```c
#include <sys/types.h>
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
```

## 2. whence

- `SEEK_SET`：从文件开头算
- `SEEK_CUR`：从当前偏移算
- `SEEK_END`：从文件末尾算

## 3. 返回值

- 成功：返回新的偏移量（`off_t`）
- 失败：返回 `-1`，设置 `errno`

## 4. 文件洞（sparse file）

如果你把偏移跳到文件末尾之后再写入，中间的“空洞”区域不会真的占用磁盘块，读取时会返回 0 字节（逻辑上全 0）。

实验：

```c
int fd = open("sparse.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
lseek(fd, 1024 * 1024, SEEK_SET);
write(fd, "X", 1);
close(fd);
```

用 `ls -l` 看逻辑大小，用 `du -h` 看实际占用。

## 5. 常见 errno

- `ESPIPE`：FD 不支持 seek（例如 pipe、socket）
- `EINVAL`：`whence` 非法
- `EBADF`：FD 无效

## 6. 练习

写程序：对普通文件与管道分别调用 `lseek`，观察错误码差异。

