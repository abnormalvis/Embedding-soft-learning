# 内存控制管理函数

## 常用接口
| 函数 | 作用 | 注意事项 |
| ---- | ---- | -------- |
| `void *malloc(size_t size);` | 分配指定字节数的未初始化内存 | 失败返回 `NULL`，不会自动清零 |
| `void *calloc(size_t count, size_t size);` | 分配并清零 count 个元素 | 若 `count*size` 溢出将返回 `NULL` |
| `void *realloc(void *ptr, size_t size);` | 调整已分配内存大小 | 可能移动内存，失败时保持原块不变 |
| `void free(void *ptr);` | 释放先前分配的内存 | 可安全地传入 `NULL` |

## 分配策略
- 将所有内存生命周期相关的操作封装成接口，便于统一管理和调试。
- 对象创建后立即初始化，避免持有悬空指针或半初始化状态。
- 大量分配/释放循环操作时可考虑内存池或对象缓存以降低碎片。

```c
int create_buffer(size_t n, int **out) {
    int *buf = malloc(sizeof(int) * n);
    if (!buf) {
        return -1;
    }
    memset(buf, 0, sizeof(int) * n);
    *out = buf;
    return 0;
}
```

## realloc 的安全用法
- 先保存旧指针：`void *tmp = realloc(ptr, new_size);`
- 检查返回值：若 `tmp == NULL`，必须保留原指针以便释放或继续使用。
- 成功时再赋值给原指针。

```c
int *resize_array(int *arr, size_t count, size_t extra) {
    size_t new_count = count + extra;
    int *tmp = realloc(arr, new_count * sizeof(int));
    if (!tmp) {
        return NULL; // 原 arr 仍然有效，由调用者决定是否释放
    }
    return tmp;
}
```

## 释放与清理
- 释放后立即将指针设为 `NULL`，迟滞释放会增加重复释放的风险。
- 为避免内存泄漏，可使用 `goto` 统一清理资源：

```c
int process(void) {
    char *buf = malloc(1024);
    FILE *fp = fopen("data.txt", "r");
    if (!buf || !fp) {
        goto cleanup;
    }
    // ... 业务逻辑 ...
cleanup:
    free(buf);
    if (fp) {
        fclose(fp);
    }
    return 0;
}
```

## 调试工具
- AddressSanitizer：编译时加入 `-fsanitize=address -fno-omit-frame-pointer` 检测越界和 UAF。
- Valgrind：运行时检测内存泄漏和无效访问，命令 `valgrind --leak-check=full ./app`。
- Electric Fence、Dr. Memory 等工具也能辅助发现问题。

## 常见陷阱
- **野指针**：指向已释放或未初始化内存，实现 `init` 函数时确保所有指针初始化为 `NULL`。
- **双重释放**：重复释放同一块内存，可在调试版中维护链表或标志位追踪。
- **内存泄漏**：忘记释放长生命周期对象；建议在模块退出或错误路径上统一释放。
- **栈溢出**：大数组应使用堆分配或声明为 `static`。
- **对齐需求**：特殊硬件或 SIMD 可能要求特定对齐，可使用 `posix_memalign` 或 `_aligned_malloc`。

## 设计模式
- **所有权语义**：明确函数是否转移、共享或借用所有权，配合文档或命名约定。
- **智能指针替代**：虽然 C 不内建智能指针，可通过结构体封装引用计数和析构函数。
- **资源表**：在系统编程中维护一个资源表（数组或链表），集中处理释放，避免遗漏。
