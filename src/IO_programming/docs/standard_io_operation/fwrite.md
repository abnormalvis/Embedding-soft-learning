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

### 参考实现

```c
#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main(void) {
    FILE *fp = fopen("/tmp/buffer_test.txt", "w");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    printf("Writing data without fflush...\n");
    printf("Time before write: %ld\n", time(NULL));
    
    // 写入数据（进入缓冲区，未必立刻写到磁盘）
    fputs("Line 1: This will be buffered\n", fp);
    fputs("Line 2: Waiting in buffer\n", fp);
    fputs("Line 3: Still buffered\n", fp);
    
    fflush(stdout);  // 确保上面的 printf 立刻输出
    
    printf("Wrote 3 lines, now sleeping 10 seconds without fflush...\n");
    printf("Check with: tail -f /tmp/buffer_test.txt\n");
    printf("Or in another terminal: cat /tmp/buffer_test.txt\n");
    
    sleep(10);
    
    printf("After sleep, calling fclose (which flushes)...\n");
    printf("Time after sleep: %ld\n", time(NULL));
    
    fclose(fp);  // 这会刷新缓冲区
    
    printf("File closed and flushed\n");
    
    return 0;
}
```

编译运行（在一个终端）：
```bash
gcc -o fwrite_demo fwrite_demo.c
./fwrite_demo &
```

在另一个终端观察：
```bash
# 在程序 sleep 期间运行
tail -f /tmp/buffer_test.txt

# 或者每秒检查一次
while true; do echo "=== $(date) ==="; cat /tmp/buffer_test.txt 2>/dev/null || echo "File not yet visible"; sleep 1; done
```

**预期行为：**
- 前 10 秒：文件可能为空或未看到数据（取决于缓冲）
- 调用 `fclose()` 后：数据立刻出现

**关键点：**
- stdio 使用缓冲以提高性能
- 缓冲刷新时机取决于缓冲类型和大小
- 对于文件通常是全缓冲（直到缓冲满或显式 flush）
- 对于终端通常是行缓冲（遇到换行时 flush）

