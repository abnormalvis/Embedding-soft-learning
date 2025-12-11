# 函数指针与回调

## 概念
- 函数在内存中同样有地址，函数指针可用来调用不同实现或实现回调机制。
- 声明形式：`返回类型 (*指针名)(参数列表)`。
- 函数名在表达式中会自动转换为指向函数的指针。

```c
typedef int (*cmp_fn)(const void *, const void *);

int compare_int(const void *a, const void *b) {
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    return (lhs > rhs) - (lhs < rhs);
}

void sort_array(int *values, size_t count, cmp_fn cmp) {
    qsort(values, count, sizeof(int), cmp);
}
```

## 常见用途
- **回调**：将具体行为委托给调用者，如事件处理、策略模式。
- **表驱动**：使用函数指针数组模拟虚函数表，实现状态机或命令派发。
- **插件机制**：运行时根据配置加载不同函数指针，实现动态行为。

## 函数指针数组
```c
typedef void (*handler_fn)(void);

void handle_start(void) { puts("start"); }
void handle_stop(void) { puts("stop"); }

handler_fn handlers[] = { handle_start, handle_stop };

void dispatch(size_t index) {
    if (index < sizeof(handlers) / sizeof(handlers[0])) {
        handlers[index]();
    }
}
```

## 指向不同签名的函数
- 函数指针类型必须匹配，被调用函数的参数和返回类型一致。
- C 允许将 `void (*)(void)` 转换为 `void (*)(int)` 但调用会导致未定义行为，必须避免。

## 回调上下文
- 在 C 中传递上下文通常使用 `void *userdata`。
- 典型例子：`qsort`、`pthread_create`、GUI 库等。

```c
typedef void (*event_cb)(int event, void *ctx);

typedef struct {
    event_cb callback;
    void *ctx;
} Notifier;

void notifier_trigger(Notifier *notifier, int event) {
    if (notifier && notifier->callback) {
        notifier->callback(event, notifier->ctx);
    }
}
```

## 函数指针与动态加载
- POSIX `dlsym` 允许在运行时从共享库获取函数指针。
- 加载流程：`dlopen` 打开共享库 → `dlsym` 获取符号 → 强制转换为适当函数指针 → `dlclose`。

```c
#include <dlfcn.h>

typedef int (*plugin_init_fn)(void);

void load_plugin(const char *path) {
    void *handle = dlopen(path, RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return;
    }
    plugin_init_fn init = (plugin_init_fn)dlsym(handle, "plugin_init");
    if (init) {
        init();
    }
    dlclose(handle);
}
```

## 安全注意
- 确保函数指针在调用前已初始化，避免调用垃圾地址。
- 在结构体中保存函数指针时，可设置默认实现或在构造函数中赋值。
- 避免跨模块使用不同调用约定的函数指针，特别是在 Windows 上需匹配 `__cdecl`、`__stdcall` 等。
