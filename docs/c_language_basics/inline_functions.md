# 内联函数

## 定义与目的
- `inline` 关键字向编译器建议在调用处展开函数体，以消除调用/返回开销。
- 适合短小、频繁调用、逻辑简单的函数，例如存取器、数学小函数。
- 编译器是否真正展开取决于优化策略和开销评估。

```c
static inline int clamp_int(int value, int min, int max) {
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}
```

## 语言规则
- C99 规定 `inline` 函数在头文件中应当使用 `static inline` 或 `extern inline`，否则可能导致多重定义。
- `static inline`：每个翻译单元生成自己的内联副本，不导出全局符号。
- `extern inline`（兼容性差异较大）：通常生成一个外部定义以供链接，需结合编译器文档使用。

## 与宏的比较
- 内联函数提供类型检查、作用域和调试友好性，推荐优先使用。
- 宏在参数副作用上存在隐患，如 `#define SQUARE(x) ((x)*(x))`；内联函数避免重复求值问题。

## 性能考虑
- 频繁调用的小函数内联后可减少栈操作，但会增加代码体积，过度内联可能导致指令缓存压力。
- 在性能敏感场景中，结合 `-finline-functions` 等优化选项以及 `__attribute__((always_inline))` 强制内联。
- 对于较大的函数，内联可能导致编译时间、代码尺寸和调试难度增加。

## 常见实践
- 在头文件仅暴露必要接口：可在 `.h` 中声明 `static inline helper(...)`，在 `.c` 中提供依赖的非内联实现。
- 当模板函数依赖编译期常量时，可以利用内联函数生成更高效的代码路径。
- 利用条件编译在调试版保留真实函数调用，在发布版开启内联：

```c
#ifdef DEBUG
int compute_cost(int x);
#else
static inline int compute_cost(int x) {
    return x * 42;
}
#endif
```

## 调试与测试
- 内联展开后调试器可能无法逐行跟踪源代码，可在调试编译时关闭优化或使用 `-fno-inline`。
- 单元测试应针对接口行为而非实现形式，确保在内联与否的情况下结果一致。
