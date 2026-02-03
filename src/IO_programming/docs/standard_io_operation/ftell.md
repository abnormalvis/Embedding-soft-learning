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

### 参考实现

```c
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    char buf[256];
    long positions[5];
    int pos_count = 0;

    printf("=== First pass: Reading and recording positions ===\n");
    
    // 第一次读取，记录多个位置
    int line_num = 0;
    while (fgets(buf, sizeof(buf), fp) != NULL && pos_count < 5) {
        positions[pos_count] = ftell(fp);
        printf("Line %d (pos=%ld): %s", line_num, positions[pos_count], buf);
        line_num++;
        pos_count++;
    }

    printf("\n=== Second pass: Seeking and re-reading ===\n");
    
    // 回到各个记录的位置重新读取
    for (int i = 0; i < pos_count; i++) {
        printf("\n--- Seeking to position %ld ---\n", positions[i]);
        
        if (fseek(fp, positions[i], SEEK_SET) != 0) {
            perror("fseek");
            fclose(fp);
            return 1;
        }
        
        long current_pos = ftell(fp);
        printf("After seek, ftell = %ld\n", current_pos);
        
        // 读取下一行
        if (fgets(buf, sizeof(buf), fp) != NULL) {
            printf("Re-read: %s", buf);
        } else {
            printf("EOF\n");
        }
    }

    fclose(fp);
    printf("\n=== Verification complete ===\n");
    
    return 0;
}
```

编译运行：
```bash
gcc -o ftell_demo ftell_demo.c
./ftell_demo /etc/passwd
```

**高级示例：文件内搜索**

```c
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <file> <search_string>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    const char *search = argv[2];
    char buf[512];
    long match_positions[10];
    int match_count = 0;

    // 第一遍：找到所有匹配位置
    while (fgets(buf, sizeof(buf), fp) != NULL && match_count < 10) {
        if (strstr(buf, search) != NULL) {
            match_positions[match_count] = ftell(fp);
            printf("Found at position: %ld\n", match_positions[match_count]);
            match_count++;
        }
    }

    printf("\nTotal matches: %d\n\n", match_count);

    // 第二遍：重新访问每个匹配位置
    printf("=== Re-reading matches ===\n");
    for (int i = 0; i < match_count; i++) {
        // 回到匹配位置前一行的开始
        fseek(fp, 0, SEEK_SET);  // 回到文件开始
        
        char line_buf[512];
        long last_pos = 0;
        
        // 逐行搜索直到达到记录的位置
        while (fgets(line_buf, sizeof(line_buf), fp) != NULL) {
            long current = ftell(fp);
            if (current >= match_positions[i]) {
                printf("Match %d: %s", i + 1, line_buf);
                break;
            }
        }
    }

    fclose(fp);
    return 0;
}
```

编译运行：
```bash
gcc -o search_demo search_demo.c
./search_demo /etc/passwd root
```

**关键点：**
- `ftell()` 返回逻辑位置（包括缓冲）
- 记录的 `ftell` 值可用于 `fseek()` 回退
- 配合 `SEEK_SET` 实现精确定位
- 对于大文件搜索，这种方法很有用
- 注意文本模式下的行结束符转换可能影响结果

