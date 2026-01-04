# 本仓库作为个人linux/C++学习仓库

## 目录结构
1. linux基础
2. C语言基础
3. C语言进阶

## C语言基础

这一部分记录了从语法到标准库的核心知识点，便于在实践中查阅和回顾。每个小节都会给出典型使用场景以及常见的坑点，帮助加深理解。

### C语言编译原理
- 典型流程：预处理(`cpp`)、编译(`cc1`)、汇编(`as`)、链接(`ld`)。使用 `gcc -E/-S/-c` 可以单独查看每个阶段的输出。
- 常用命令：`gcc -E main.c -o main.i` 查看宏展开；`gcc -S main.c` 得到汇编；`gcc -c main.c` 只生成目标文件；`gcc main.o utils.o -o app` 进行链接。
- 了解链接过程中的符号解析、静态/动态库搜索顺序，为排查 "undefined reference" 等问题打基础。

### C数据结构
- `struct` 用于描述组合数据，合理安排成员顺序可以减少内存填充字节。
- `union` 成员共享同一段内存，常用于解析通信协议、实现类型擦除。
- `enum` 提升代码可读性，可与 `typedef` 配合定义语义化的类型别名。
- `bit-field` 可以在结构体中压缩布尔或小范围整数，但要注意不同编译器的布局差异。

```c
typedef struct {
   int id;
   char name[32];
   float score;
} Student;

void update_score(Student *stu, float delta) {
   if (!stu) {
      return;
   }
   stu->score += delta;
}
```

### 进制转换
- 输出时可以使用格式控制符：`printf("%d %o %x", n, n, n);` 分别得到十进制、八进制和十六进制表示。
- 字符串转整数推荐 `strtol`/`strtoul`，可通过基数参数指定输入进制并捕获非法字符。
- 自定义转换时可用反复除基数取余的方法构造字符串，注意处理负数和 `NUL` 终止符。

```c
unsigned from_hex(const char *text) {
   return (unsigned)strtoul(text, NULL, 16);
}
```

### 标准输入输出
- `stdio.h` 提供带缓冲的文件流接口，常用的有 `printf`/`fprintf`、`scanf`/`fscanf`、`fgets`、`fread`/`fwrite`。
- 行缓冲(stdout)在连接终端时会在换行后刷新；需要立即输出时可调用 `fflush(stdout);`。
- 从安全角度优先使用 `fgets` 替代 `gets`，读取二进制数据时应选择 `fread`/`fwrite` 并检查返回值。

### 操作运算符
- 算术、关系、逻辑、位运算、赋值、条件、逗号、指针等类别。运算符优先级会直接影响表达式结果，建议适时加括号。
- 注意短路逻辑(`&&`/`||`)只会执行必要的分支；位运算常用于掩码和标志位；`->` 用于结构体指针访问成员。
- 自增自减的前缀和后缀形式在表达式求值顺序复杂时易出错，应避免在同一表达式中重复修改同一变量。

### 内联函数
- 通过 `inline` 关键字提示编译器将函数体展开以减少调用开销，适用于体积小、调用频繁的函数。
- C99 中建议与 `static inline` 搭配使用，避免头文件定义的函数在多个翻译单元中重复定义。
- 与宏相比，内联函数具备类型检查和作用域控制，更安全；但编译器是否真正展开仍取决于优化策略。

### 变参函数介绍
- 依赖 `stdarg.h` 提供的 `va_list`、`va_start`、`va_arg`、`va_end` 宏逐个读取参数。
- 变参函数至少需要一个固定参数，用于传递参数数量或格式信息。
- 需要确保读取顺序和实际传参类型一致，否则会触发未定义行为。

```c
int sum_ints(int count, ...) {
   int total = 0;
   va_list args;
   va_start(args, count);
   for (int i = 0; i < count; ++i) {
      total += va_arg(args, int);
   }
   va_end(args);
   return total;
}
```

### libc库常用字符串函数
- 字符串以 `\0` 结尾，所有函数都假设输入指针指向合法的可访问内存，使用前务必检查缓冲区大小并确保以 `\0` 终止。
- 重新实现这些函数可以强化对指针运算、边界条件的掌握，便于在嵌入式或受限环境下定制更安全的版本。

#### strlen()
- 原型：`size_t strlen(const char *s);` 返回首个 `\0` 前的字符数。
- 不包含尾部的 `\0`，遇到未终止的字符串会越界读取。
- 常用于计算缓冲区长度，但若后续立即复制数据，建议直接使用 `strnlen` 等更安全的替代品。

#### strcpy()
- 原型：`char *strcpy(char *dest, const char *src);` 将源字符串连同 `\0` 复制到目标缓冲区。
- 目标缓冲区必须足够大，否则导致溢出；通常搭配 `strncpy` 或手动检查长度。

#### strcat()
- 原型：`char *strcat(char *dest, const char *src);` 追加字符串到目标尾部。
- 使用前需要确保 `dest` 已含有 `\0` 并且剩余空间足够，追加操作为 O(n)。

#### strcmp()
- 原型：`int strcmp(const char *lhs, const char *rhs);` 按字典序比较两个字符串。
- 返回负数、零、正数分别表示小于、等于、大于。比较过程中遇到第一个不同字符或 `\0` 即停止。

#### strstr()
- 原型：`char *strstr(const char *haystack, const char *needle);` 查找子串首次出现的位置。
- 若找到则返回指向 `haystack` 中匹配位置的指针，未找到返回 `NULL`。
- 朴素实现是 O(n*m)，大数据场景可考虑 KMP、Boyer-Moore 等替代算法。

#### strchr()
- 原型：`char *strchr(const char *s, int c);` 查找字符首次出现位置，包括搜索 `\0`。
- 返回指针可直接用于后续操作，如分割字符串或计算偏移。

#### strrchr()
- 原型：`char *strrchr(const char *s, int c);` 查找字符最后一次出现位置。
- 当需要定位文件路径中的最后一个分隔符时非常常用。

#### strtok()
- 原型：`char *strtok(char *str, const char *delim);` 使用分隔符集合切分字符串。
- 内部使用静态指针保存上下文，不是线程安全；可用 `strtok_r` 替代。
- 原始字符串会被 `\0` 替换，若需要保留原始数据应先复制副本。

```c
char buffer[] = "PATH=/usr/bin:/bin:/usr/local/bin";
for (char *token = strtok(buffer, "=:"); token; token = strtok(NULL, "=:") ) {
   puts(token);
}
```

### 内存控制管理函数
- 主要包括 `malloc`、`calloc`、`realloc`、`free`。
- `malloc` 分配未初始化的内存，需要检查返回值是否为 `NULL`；`calloc` 会自动清零，适合数组初始化。
- `realloc` 可能移动内存，调用前保存原指针，防止在失败时丢失原数据。
- 释放内存后将指针设置为 `NULL` 可降低悬空指针风险，配合内存调试工具(`valgrind`)检查泄漏。

```c
int *values = malloc(sizeof(int) * count);
if (!values) {
   return -1;
}

memset(values, 0, sizeof(int) * count);

int *expanded = realloc(values, sizeof(int) * (count + 10));
if (!expanded) {
   free(values);
   return -1;
}
values = expanded;

free(values);
values = NULL;
```

## C语言进阶

这一部分深入探讨 C 语言的内存模型、指针机制、复合数据类型以及程序运行时的内存布局，为系统编程和性能优化打下基础。

### 一维数组语法使用和内存剖析
- 数组在内存中连续存储，下标从 0 开始，通过指针算术高效访问。
- 数组名退化为指向首元素的指针，`sizeof(array)` 返回整个数组大小。
- 适用场景：数据缓冲区、查找表、栈模拟等固定大小的数据集合。
- 📄 [详细文档](docs/c_language_advanced/one_dimensional_array.md)
- 💻 [示例代码](src/c_language_advanced/arrays/array_demo.c)

### 二维数组语法使用和内存剖析
- 二维数组按行优先顺序在内存中连续分布，`array[i][j]` 等价于 `*(*(array + i) + j)`。
- 传递给函数时必须指定列数，或使用一维指针手动计算偏移。
- 适用场景：矩阵运算、游戏棋盘、DP 表、图像像素数组。
- 📄 [详细文档](docs/c_language_advanced/two_dimensional_array.md)
- 💻 [示例代码](src/c_language_advanced/arrays/array_demo.c)

### 复合数据类型
- 结构体(`struct`)、联合体(`union`)、枚举(`enum`) 提供组合数据的能力。
- `typedef` 简化类型定义，嵌套复合类型支持复杂数据建模。
- 适用场景：配置数据、协议解析、状态机、面向对象模拟。
- 📄 [详细文档](docs/c_language_advanced/compound_data_types.md)

### 结构体深度解析
- 内存布局受对齐规则影响，合理安排成员顺序可减少填充字节。
- 结构体嵌套分为内嵌（直接包含）和外挂（指针引用），各有空间与灵活性权衡。
- 柔性数组成员允许结构体尾部动态长度数组，减少内存分配次数。
- 📄 [详细文档](docs/c_language_advanced/struct_deep_dive.md)
- 💻 [示例代码](src/c_language_advanced/structs/struct_demo.c)

### 指针全面解析
- **地址操作**：`&` 取址、`*` 解引用，指针本身也是变量。
- **指针偏移**：`ptr + n` 移动 `n * sizeof(*ptr)` 字节，常用于数组遍历。
- **指针与数组**：数组名退化为指针，`arr[i]` 等价于 `*(arr + i)`。
- **指针与结构体**：使用 `->` 访问成员，等价于 `(*ptr).member`。
- **指针与函数**：传递指针修改外部变量，返回指针需注意生命周期。
- **函数指针与回调**：存储函数地址，实现策略模式和事件驱动。
- **指针与 const**：`const int *` 保护内容，`int * const` 保护指针。
- **多级指针**：`int **` 指向指针的指针，用于动态二维数组或修改指针本身。
- 📄 [详细文档](docs/c_language_advanced/pointer_deep_dive.md)
- 💻 [示例代码](src/c_language_advanced/pointers/pointer_demo.c)

### 内存空间整体分布
- 进程内存分为代码段、数据段、BSS段、堆、栈五大区域。
- **代码段**：存储指令和常量字符串，只读可共享。
- **数据段**：已初始化全局/静态变量。
- **BSS段**：未初始化或零值全局/静态变量，不占用磁盘空间。
- **栈**：局部变量、函数参数、返回地址，自动管理，大小有限。
- **堆**：动态分配内存，需手动 `malloc`/`free`，生命周期灵活。
- 📄 [详细文档](docs/c_language_advanced/memory_layout.md)
- 💻 [示例代码](src/c_language_advanced/memory/memory_layout_demo.c)

## 编译与运行示例

### C 语言基础示例
```bash
# 编译单个示例
gcc src/c_language_basics/base_conversion/convert.c -o convert
./convert FF 16

# 编译内联函数示例
gcc src/c_language_basics/inline_functions/demo.c -o inline_demo
./inline_demo
```

### C 语言进阶示例
```bash
cd src/c_language_advanced

# 编译所有示例
make all

# 运行单个示例
./array_demo
./struct_demo
./pointer_demo
./memory_demo

# 清理
make clean
```

### Linux 基础示例
```bash
# 运行 Shell 脚本
chmod +x src/linux_basics/shell_and_filesystem/filesystem_demo.sh
./src/linux_basics/shell_and_filesystem/filesystem_demo.sh

# 编译调试工具示例
cd src/linux_basics/build_and_debug_tools
make
./demo
```

## 数据结构

### 编译运行测试
```bash
cd src/Data_Structure
make
./build $(可执行文件)

# make clean # 清理
``` 
### 顺序表

### 链表

- 可改进方向：把尾插法改为头插法, 提高插入的效率
- 添加虚拟头节点`dummy-head`，`dummy-head`不存储数据
### 双向链表

### 内核链表

### 栈符合结构

### 队列结构

### 普通树结构

### 二叉树结构

### 二叉查找树结构

### 红黑树结构
## 日志