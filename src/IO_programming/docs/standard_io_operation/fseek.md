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

### 参考实现

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    // 先获取文件大小
    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    printf("File size: %ld bytes\n", filesize);

    // 计算要读取的字节数
    long bytes_to_read = (filesize > 100) ? 100 : filesize;
    long offset = filesize - bytes_to_read;

    printf("Reading last %ld bytes (offset: %ld)\n", bytes_to_read, offset);

    // 移动到指定位置
    if (fseek(fp, offset, SEEK_SET) != 0) {
        perror("fseek");
        fclose(fp);
        return 1;
    }

    // 读取数据
    char *buffer = (char *)malloc(bytes_to_read);
    if (!buffer) {
        perror("malloc");
        fclose(fp);
        return 1;
    }

    size_t n = fread(buffer, 1, bytes_to_read, fp);
    if (n != bytes_to_read && !feof(fp)) {
        perror("fread");
        free(buffer);
        fclose(fp);
        return 1;
    }

    printf("\n=== Last %zu bytes ===\n", n);
    
    // 打印（假设是文本，但也要处理二进制）
    for (size_t i = 0; i < n; i++) {
        if (buffer[i] >= 32 && buffer[i] < 127) {
            putchar(buffer[i]);
        } else {
            printf("[%02x]", (unsigned char)buffer[i]);
        }
    }
    printf("\n");

    free(buffer);
    fclose(fp);

    return 0;
}
```

编译运行：
```bash
gcc -o tail100 tail100.c
./tail100 /etc/passwd
./tail100 /tmp/some_file
```

**另一个版本：使用相对偏移**

```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "rb");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    // 直接从末尾前 100 字节处寻址
    if (fseek(fp, -100, SEEK_END) != 0) {
        // 文件可能小于 100 字节
        rewind(fp);  // 回到开始
        printf("File smaller than 100 bytes, reading from start\n");
    }

    char buf[4096];
    size_t n;
    printf("=== Last up to 100 bytes ===\n");
    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
        fwrite(buf, 1, n, stdout);
    }
    printf("\n");

    fclose(fp);
    return 0;
}
```

**关键点：**
- `fseek(..., -100, SEEK_END)` 定位到末尾往前 100 字节
- 文件小于 100 字节时会失败，需要检查并回到开始
- `ftell()` 配合 `fseek()` 可以实现位置记录和回退
- 二进制文件使用 `"rb"` 模式避免换行转换问题

