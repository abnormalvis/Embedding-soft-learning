# 预处理器高级用法

## 宏与替换
- 宏通过 `#define` 定义，编译前执行文本替换。
- 带参数宏使用 `#define SQUARE(x) ((x) * (x))`，应注意重复求值问题。
- 使用 `do { ... } while (0)` 包裹多语句宏，确保语法安全。

## 条件编译
- `#if/#elif/#else/#endif` 控制不同配置下的编译路径。
- `#ifdef FEATURE` 等价于 `#if defined(FEATURE)`；反义 `#ifndef`。
- `#if 0` 常用于临时屏蔽代码，但提交前应清理。

```c
#if defined(DEBUG)
    #define LOG_DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
    #define LOG_DEBUG(...) ((void)0)
#endif
```

## 宏函数技巧
- 字符串化：`#` 将参数转为字符串字面量。
  ```c
  #define STR(x) #x
  printf("%s\n", STR(hello)); // 输出 "hello"
  ```
- 连接：`##` 拼接标识符。
  ```c
  #define MAKE_FUNC(prefix, name) prefix##_##name
  void MAKE_FUNC(net, init)(void); // 生成 net_init
  ```

## 可变参数宏
- 语法：`#define LOG(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)`
- `__VA_ARGS__` 展开所有传入的可变参数，GNU 扩展支持 `##__VA_ARGS__` 处理空参数。

## 内建宏
- `__FILE__`、`__LINE__`、`__func__` 提供位置信息。
- `__DATE__`、`__TIME__`、`__TIMESTAMP__` 给出编译时间戳（可影响可重现构建）。
- `__COUNTER__`：GCC/Clang 扩展，每次展开递增，可用于生成唯一标识符。

## pragma 与诊断
- `#pragma once` 防止头文件重复包含。
- `#pragma GCC diagnostic push/pop` 调整局部警告级别。
- `#pragma pack(n)` 修改结构体对齐方式，使用后及时恢复。

```c
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
static void helper(int unused) {
}
#pragma GCC diagnostic pop
```

## 生成代码与模板
- 预处理器可生成重复代码，如状态表、命令列表。
- 示例：使用 X 宏维护列表，减少重复维护：

```c
#define ERROR_TABLE \
    X(ERR_OK, "ok") \
    X(ERR_IO, "io error") \
    X(ERR_CONFIG, "config error")

typedef enum {
#define X(name, msg) name,
    ERROR_TABLE
#undef X
} ErrorCode;

static inline const char *error_to_string(ErrorCode code) {
    switch (code) {
#define X(name, msg) case name: return msg;
        ERROR_TABLE
#undef X
    default:
        return "unknown";
    }
}
```

## 调试预处理输出
- 使用 `gcc -E file.c` 查看宏展开及包含文件效果。
- `clang -E -dM -` 输出所有内建宏与编译器定义的宏。
- 在生成代码场景中检查展开结果可避免难以调试的宏错误。

## 限制与注意
- 预处理器不理解 C 语法，错误常在编译阶段暴露。
- 避免滥用宏，优先考虑 `static inline` 函数或 `enum` 常量。
- 复杂宏需详细注释并配套测试，便于维护和理解。
