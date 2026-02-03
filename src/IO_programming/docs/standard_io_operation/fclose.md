# fclose：关闭流并刷新缓冲

`fclose(3)` 关闭 `FILE*` 流，必要时会先刷新输出缓冲（等价于隐式 `fflush`），并释放相关资源。

## 1. 头文件与函数原型

```c
#include <stdio.h>

int fclose(FILE *stream);
```

## 2. 返回值

- 成功：返回 `0`
- 失败：返回 `EOF`（通常是 `-1`），并设置 `errno`

## 3. 注意事项

- `fclose` 失败通常意味着底层写入失败（磁盘满、NFS 问题等）。对关键数据，应检查返回值。
- 不要对同一个 `FILE*` 调用两次 `fclose`。

## 4. 练习

写程序：向文件写入大量数据后 `fclose`，并在磁盘满/权限不足的情况下观察错误信息（可在受控环境模拟）。

### 参考实现

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    const char *filename = "/tmp/fclose_test.txt";
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    printf("Writing large amount of data...\n");
    
    // 写入 1MB 数据
    char buffer[1024];
    memset(buffer, 'X', sizeof(buffer));
    
    for (int i = 0; i < 1024; i++) {
        size_t n = fwrite(buffer, 1, sizeof(buffer), fp);
        if (n < sizeof(buffer)) {
            printf("Short write at iteration %d\n", i);
            if (ferror(fp)) {
                perror("write error during fwrite");
            }
            break;
        }
    }

    printf("Calling fclose...\n");
    
    // fclose 会尝试刷新缓冲，这是可能发生错误的地方
    if (fclose(fp) != 0) {
        perror("fclose failed");
        fprintf(stderr, "fclose returned EOF, possible write error or flush failure\n");
        return 1;
    }

    printf("fclose succeeded\n");
    printf("Data successfully written to %s\n", filename);
    
    return 0;
}
```

编译运行：
```bash
gcc -o fclose_demo fclose_demo.c
./fclose_demo
echo "Return code: $?"
```

**模拟磁盘满错误（可选）：**

```bash
# 使用 tmpfs 限制大小来模拟磁盘满
mkdir -p /tmp/small_disk
sudo mount -t tmpfs -o size=1M tmpfs /tmp/small_disk

# 修改代码中的 filename 为 /tmp/small_disk/test.txt
gcc -o fclose_demo fclose_demo.c
./fclose_demo

# 清理
sudo umount /tmp/small_disk
```

**权限不足的演示：**

```bash
# 创建只读目录
mkdir -p /tmp/readonly
touch /tmp/readonly/test.txt
chmod 444 /tmp/readonly  # 移除写权限

# 修改代码尝试写入 /tmp/readonly/test.txt
# 会在 fopen 或 fclose 时失败
```

**关键点：**
- `fclose()` 不仅关闭 FD，还会刷新缓冲区
- 刷新时发生的错误（磁盘满、权限问题等）会在 `fclose()` 返回时报告
- 对于关键数据，应检查 `fclose()` 的返回值
- 错误处理应该包括检查 `ferror()` 和 `feof()`

