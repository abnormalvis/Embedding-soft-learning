# 模块化设计与头文件管理

## 头文件的角色
- 声明函数、类型、宏、常量，让多个源文件共享接口。
- 使用 include guard：

```c
#ifndef PROJECT_MODULE_H
#define PROJECT_MODULE_H
// 声明内容
#endif
```

- C23 引入 `#embed`、`#warning` 等扩展，不过兼容性有限。

## 分离声明与实现
- `.h` 提供接口声明，`.c` 实现具体逻辑。
- 避免在头文件中定义非 `static` 函数或全局变量，以防多重定义。
- 对于只在本文件使用的函数或变量使用 `static` 限定。

## 信息隐藏
- 使用不完整类型隐藏结构体实现细节：

```c
// foo.h
typedef struct Foo Foo;
Foo *foo_create(void);
void foo_destroy(Foo *foo);

// foo.c
struct Foo {
    int count;
    char *buffer;
};
```

- 调用者只能通过接口操作对象，内部布局可随时修改。

## 依赖管理
- 减少头文件相互包含，使用前向声明打破循环依赖。
- `#include` 顺序：1) 该文件对应的头文件 2) 系统头文件 3) 其他项目头文件。
- 启用 `-I` 添加头文件搜索路径，尽量使用相对路径或项目内根路径便于移植。

## 接口设计原则
- 函数命名包含模块前缀，例如 `net_init`、`net_connect`，避免符号冲突。
- 使用枚举或错误码统一返回值，提供 `enum module_error` 与 `const char *module_strerror`。
- 接口对参数的要求（所有权、可否为 NULL、线程安全性）需要文档说明。

## 内联与宏
- 在头文件中提供 `static inline` 工具函数以减少重复代码。
- 宏适合常量表达式或编译期条件，但要关注副作用。
- 利用 `do { ... } while (0)` 模式实现安全宏：

```c
#define LOG_INFO(fmt, ...) \
    do { fprintf(stderr, "[INFO] " fmt "\n", __VA_ARGS__); } while (0)
```

## 构建组织
- 使用 `make`、`cmake` 或其他构建系统定义模块的依赖关系。
- 编译参数考虑：`-Wall -Wextra -pedantic` 提高警告等级，`-fvisibility=hidden` 限制符号导出。
- 如果要生成动态库，在头文件中使用导出宏控制符号：

```c
#if defined(_WIN32)
  #ifdef BUILD_LIB
    #define API __declspec(dllexport)
  #else
    #define API __declspec(dllimport)
  #endif
#else
  #define API __attribute__((visibility("default")))
#endif
```

## 文档与测试
- 头文件可通过注释或 Doxygen 风格文档描述函数用途、参数、返回值。
- 每个模块提供独立的单元测试，验证公开接口的行为。
- 对外暴露的头文件应保持稳定，内部头文件放在 `include/project/internal/` 等路径。
