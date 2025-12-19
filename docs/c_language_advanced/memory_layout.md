# 内存空间布局

## 进程内存分区概述
C 程序运行时的内存分为以下几个区域，每个区域都有其特定的用途和特性：

```
高地址（0xFFFFFFFF 在 32 位系统）
┌──────────────┐
│  命令行参数  │  环境变量（argv, envp）
│  和环境变量  │  由操作系统传递
├──────────────┤
│     栈       │  ↓ 向下增长（从高地址到低地址）
│   (Stack)    │  • 局部变量
│              │  • 函数参数
│              │  • 返回地址
│              │  • 保存的寄存器
│              │  • 栈帧指针
├──────────────┤
│      ↕       │  未分配空间
│  (未使用)    │  栈和堆之间的空闲区域
│      ↕       │  可以动态增长
├──────────────┤
│     堆       │  ↑ 向上增长（从低地址到高地址）
│   (Heap)     │  • malloc/calloc/realloc 分配
│              │  • 动态分配的数据结构
│              │  • 需要手动管理
├──────────────┤
│  BSS 段      │  未初始化数据段
│ (Block       │  • 未初始化的全局变量
│  Started by  │  • 未初始化的静态变量
│  Symbol)     │  • 初始化为 0 的全局/静态变量
│              │  • 程序启动时自动清零
├──────────────┤
│  数据段      │  已初始化数据段
│  (Data       │  • 已初始化的全局变量
│   Segment)   │  • 已初始化的静态变量
│              │  • 分为只读和可读写两部分
├──────────────┤
│  代码段      │  文本段（Text Segment）
│  (Text       │  • 程序的机器指令
│   Segment)   │  • 只读、可共享
│              │  • 字符串常量
│              │  • const 全局数据
└──────────────┘
低地址（0x00000000）
```

### 内存布局的重要特性
- **地址空间隔离**：每个进程都有独立的虚拟地址空间
- **内存保护**：不同区域有不同的访问权限（读/写/执行）
- **动态增长**：栈和堆可以在运行时动态增长
- **地址随机化**：现代操作系统使用 ASLR（地址空间布局随机化）提高安全性

## 代码段（Text Segment）

### 特性
- **存储内容**：程序的机器指令（编译后的二进制代码）
- **访问权限**：只读（Read-Only），可执行（Executable）
- **共享性**：多个进程可以共享同一个程序的代码段，节省内存
- **大小固定**：在程序编译时确定，运行时不变
- **位置**：通常位于内存的低地址区域

### 存储内容详解
1. **函数代码**：所有函数的机器指令
2. **字符串常量**：使用双引号定义的字符串字面量
3. **const 全局变量**：某些编译器会将 const 全局变量放在代码段

```c
#include <stdio.h>

// 函数代码存储在代码段
void print_message() {
    printf("Hello, World!\n");
}

int main() {
    // "hello" 字符串字面量位于代码段（只读）
    const char *msg = "hello";
    
    // 尝试修改代码段的内容会导致段错误
    // msg[0] = 'H';  // 运行时错误：Segmentation Fault
    
    // 字符数组在栈上，可以修改
    char array[] = "hello";
    array[0] = 'H';  // 正确：修改栈上的数据
    
    printf("String literal: %s\n", msg);
    printf("Array: %s\n", array);
    
    return 0;
}
```

### 为什么代码段是只读的？
1. **安全性**：防止程序意外或恶意修改自己的代码
2. **共享**：多个进程可以共享同一份代码，节省物理内存
3. **稳定性**：避免代码被破坏导致程序崩溃

### 查看代码段
```bash
# 使用 objdump 查看反汇编代码
objdump -d program

# 查看程序各段信息
readelf -S program

# 查看程序运行时的内存映射
cat /proc/<pid>/maps | grep r-xp
```

## 数据段（Data Segment）

### 特性
- **存储内容**：已初始化的全局变量和静态变量（初始值非零）
- **访问权限**：可读可写（Read-Write）
- **生命周期**：整个程序运行期间
- **大小**：编译时确定，记录在可执行文件中
- **分类**：
  - **只读数据段（.rodata）**：存储 const 全局变量和常量
  - **读写数据段（.data）**：存储可修改的全局变量

### 存储内容详解
```c
#include <stdio.h>

// 以下变量都存储在数据段
int global_var = 100;              // 全局变量（可读写数据段）
static int static_var = 200;       // 静态变量（可读写数据段）
const int const_global = 300;      // const 全局变量（只读数据段）

// 字符串字面量在代码段，但指针在数据段
char *global_ptr = "constant string";  // 指针在数据段，字符串在代码段

void function() {
    // 静态局部变量也在数据段
    static int counter = 0;
    counter++;
    printf("Counter: %d\n", counter);
}

int main() {
    printf("global_var address: %p\n", (void*)&global_var);
    printf("static_var address: %p\n", (void*)&static_var);
    printf("const_global address: %p\n", (void*)&const_global);
    
    function();  // Counter: 1
    function();  // Counter: 2（静态变量保持状态）
    
    return 0;
}
```

### 数据段 vs BSS 段
| 特性 | 数据段 (.data) | BSS 段 (.bss) |
|------|----------------|---------------|
| 初始化 | 已初始化（非零值） | 未初始化或初始化为 0 |
| 文件大小 | 占用可执行文件空间 | 不占用可执行文件空间 |
| 加载时间 | 从文件加载初始值 | 运行时由系统清零 |
| 典型内容 | `int x = 10;` | `int x;` 或 `int x = 0;` |

### 实际例子
```c
#include <stdio.h>

// 数据段：占用可执行文件空间
int initialized_array[1000] = {1, 2, 3};  // 初始化部分值

// BSS 段：不占用可执行文件空间
int uninitialized_array[1000];            // 未初始化

int main() {
    printf("Size of initialized_array: %zu bytes\n", sizeof(initialized_array));
    printf("Size of uninitialized_array: %zu bytes\n", sizeof(uninitialized_array));
    
    // 编译后检查文件大小，会发现 initialized_array 使文件更大
    return 0;
}
```

### 查看数据段
```bash
# 查看各段大小
size program

# 查看详细段信息
readelf -S program | grep -E "\.data|\.rodata"

# 运行时查看
cat /proc/<pid>/maps | grep rw-p
```

## BSS 段（Block Started by Symbol）

### 特性
- **存储内容**：未初始化或初始化为 0 的全局变量和静态变量
- **访问权限**：可读可写（Read-Write）
- **文件大小**：不占用可执行文件空间（只记录大小信息）
- **初始化**：程序加载时由操作系统自动清零
- **优势**：节省磁盘空间和加载时间

### 为什么需要 BSS 段？
如果一个程序有大量初始化为 0 的全局变量，如果都放在数据段，可执行文件会非常大。BSS 段只记录这些变量需要多少空间，实际的零值在程序加载时由操作系统填充。

```c
#include <stdio.h>

// BSS 段：未初始化，程序启动时自动初始化为 0
int uninitialized_global;
static int static_zero;
int zero_array[10000];  // 10000 个整数，都会初始化为 0

// 数据段：显式初始化为非零值
int initialized_global = 42;
int nonzero_array[10000] = {1};  // 第一个元素为 1，其余为 0

void display_values() {
    printf("uninitialized_global: %d\n", uninitialized_global);  // 0
    printf("static_zero: %d\n", static_zero);                    // 0
    printf("initialized_global: %d\n", initialized_global);      // 42
}

int main() {
    display_values();
    
    // 验证 zero_array 确实被初始化为 0
    int all_zero = 1;
    for (int i = 0; i < 10000; i++) {
        if (zero_array[i] != 0) {
            all_zero = 0;
            break;
        }
    }
    printf("zero_array all zeros: %s\n", all_zero ? "Yes" : "No");
    
    return 0;
}
```

### BSS 段的优化效果
```c
// 示例：比较文件大小

// 程序 1：使用 BSS 段
int large_array[1000000];  // 不占用文件空间

// 程序 2：使用数据段
int large_array[1000000] = {0};  // 占用约 4MB 文件空间
```

编译后：
```bash
# 程序 1 的可执行文件很小
gcc -o prog1 prog1.c
ls -lh prog1  # 可能只有几 KB

# 程序 2 的可执行文件很大
gcc -o prog2 prog2.c
ls -lh prog2  # 可能有几 MB
```

### 静态局部变量
```c
#include <stdio.h>

void count_calls() {
    // 静态局部变量在 BSS 段（未显式初始化，默认为 0）
    static int call_count;
    call_count++;
    printf("This function has been called %d times\n", call_count);
}

void explicit_init() {
    // 显式初始化的静态局部变量在数据段
    static int counter = 100;
    counter++;
    printf("Counter: %d\n", counter);
}

int main() {
    count_calls();  // 1
    count_calls();  // 2
    count_calls();  // 3
    
    explicit_init();  // 101
    explicit_init();  // 102
    
    return 0;
}
```

### 查看 BSS 段
```bash
# 查看 BSS 段大小
size program
# 输出：text    data    bss    dec    hex filename

# 详细查看
readelf -S program | grep .bss

# nm 命令查看 BSS 段符号（B 或 b 标记）
nm program | grep " [Bb] "
```

## 栈（Stack）

### 特性
- **存储内容**：局部变量、函数参数、返回地址、保存的寄存器
- **访问权限**：可读可写（Read-Write）
- **增长方向**：从高地址向低地址增长（向下增长）
- **分配方式**：自动分配和释放，由编译器管理
- **数据结构**：后进先出（LIFO - Last In First Out）
- **大小限制**：通常几 MB（Linux 默认 8MB）
- **速度**：非常快，只需移动栈指针

### 栈的工作原理
```c
#include <stdio.h>

void level3(int c) {
    int local3 = 30;
    printf("level3: local3=%d, address=%p\n", local3, (void*)&local3);
    printf("level3: param c=%d, address=%p\n", c, (void*)&c);
}

void level2(int b) {
    int local2 = 20;
    printf("level2: local2=%d, address=%p\n", local2, (void*)&local2);
    level3(b + 1);
    printf("level2: returning\n");
}

void level1(int a) {
    int local1 = 10;
    printf("level1: local1=%d, address=%p\n", local1, (void*)&local1);
    level2(a + 1);
    printf("level1: returning\n");
}

int main() {
    int main_local = 0;
    printf("main: main_local=%d, address=%p\n", main_local, (void*)&main_local);
    level1(1);
    printf("main: returning\n");
    return 0;
}
```

运行输出会显示地址从高到低分配（每次函数调用，栈向低地址增长）。

### 栈帧（Stack Frame）详解

每次函数调用都会创建一个栈帧，包含以下内容：

```
栈帧结构（从高地址到低地址）：
┌─────────────────┐  ← 高地址
│  函数参数       │  参数从右到左入栈
├─────────────────┤
│  返回地址       │  函数返回后继续执行的位置
├─────────────────┤
│  旧的帧指针     │  保存调用者的栈帧位置
├─────────────────┤  ← 当前帧指针（BP/FP）
│  局部变量       │  函数内声明的变量
├─────────────────┤
│  保存的寄存器   │  需要保留的寄存器值
├─────────────────┤
│  临时数据       │  表达式计算的中间结果
└─────────────────┘  ← 栈指针（SP），低地址
```

示例代码：
```c
void func(int param1, int param2) {
    int local1 = 10;
    char buffer[100];
    int local2 = 20;
    
    // 此时栈帧包含：
    // - param1, param2（参数）
    // - 返回地址
    // - 旧的帧指针
    // - local1（局部变量）
    // - buffer[100]（局部数组）
    // - local2（局部变量）
}
```

### 栈的优势
1. **速度快**：分配和释放只需调整栈指针，O(1) 时间复杂度
2. **自动管理**：不需要手动释放内存，函数返回时自动回收
3. **缓存友好**：连续的内存访问，CPU 缓存命中率高
4. **无碎片**：不会产生内存碎片

### 栈的限制
1. **大小有限**：通常只有几 MB
2. **生命周期短**：函数返回后数据无效
3. **不能返回局部变量地址**：数据会被覆盖

```c
// 危险！返回局部变量的地址
int* dangerous_function() {
    int local = 42;
    return &local;  // 错误：local 在函数返回后被销毁
}

int main() {
    int *ptr = dangerous_function();
    // *ptr 指向已经无效的内存
    printf("%d\n", *ptr);  // 未定义行为
    return 0;
}
```

### 查看和修改栈大小
```bash
# 查看当前栈大小限制（单位：KB）
ulimit -s

# 设置栈大小为 16MB
ulimit -s 16384

# 无限制（不推荐）
ulimit -s unlimited

# 编译时指定栈大小（GCC）
gcc -Wl,-z,stack-size=16777216 program.c
```

### 栈的实际应用
```c
#include <stdio.h>

// 使用栈实现简单的临时计算
int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n-1) + fibonacci(n-2);  // 每次递归都创建新栈帧
}

// 栈上分配小数组（快速）
void process_data() {
    int data[100];  // 在栈上，400 字节
    for (int i = 0; i < 100; i++) {
        data[i] = i * i;
    }
}

int main() {
    printf("fibonacci(10) = %d\n", fibonacci(10));
    process_data();
    return 0;
}
```

## 堆（Heap）

### 特性
- **存储内容**：动态分配的数据（malloc/calloc/realloc 申请的内存）
- **访问权限**：可读可写（Read-Write）
- **增长方向**：从低地址向高地址增长（向上增长）
- **分配方式**：手动分配和释放，程序员负责管理
- **大小**：通常很大，受系统物理内存和虚拟内存限制
- **速度**：相对较慢，需要复杂的内存管理算法
- **生命周期**：由程序员控制，可以跨函数使用

### 堆内存分配函数

#### malloc - 分配未初始化的内存
```c
#include <stdio.h>
#include <stdlib.h>

void malloc_example() {
    // 分配 100 个整数的内存（未初始化，内容不确定）
    int *arr = malloc(100 * sizeof(int));
    
    // 必须检查分配是否成功
    if (arr == NULL) {
        perror("malloc failed");
        return;
    }
    
    // 使用内存
    for (int i = 0; i < 100; i++) {
        arr[i] = i;
    }
    
    // 必须释放内存
    free(arr);
    arr = NULL;  // 良好习惯：避免悬空指针
}
```

#### calloc - 分配并清零的内存
```c
void calloc_example() {
    // 分配 100 个整数的内存，并全部初始化为 0
    int *arr = calloc(100, sizeof(int));
    
    if (arr == NULL) {
        perror("calloc failed");
        return;
    }
    
    // 所有元素都已经是 0
    printf("arr[0] = %d\n", arr[0]);  // 0
    printf("arr[99] = %d\n", arr[99]); // 0
    
    free(arr);
}
```

#### realloc - 调整已分配内存的大小
```c
void realloc_example() {
    // 初始分配 10 个整数
    int *arr = malloc(10 * sizeof(int));
    if (arr == NULL) return;
    
    for (int i = 0; i < 10; i++) {
        arr[i] = i;
    }
    
    // 扩展到 20 个整数
    int *new_arr = realloc(arr, 20 * sizeof(int));
    if (new_arr == NULL) {
        // realloc 失败，原内存仍然有效
        free(arr);
        return;
    }
    
    // 更新指针
    arr = new_arr;
    
    // 前 10 个元素保持不变，后 10 个未初始化
    printf("arr[0] = %d\n", arr[0]);   // 0
    printf("arr[9] = %d\n", arr[9]);   // 9
    
    // 初始化新分配的部分
    for (int i = 10; i < 20; i++) {
        arr[i] = i;
    }
    
    free(arr);
}
```

### 堆 vs 栈对比

| 特性 | 栈（Stack） | 堆（Heap） |
|------|-------------|-----------|
| **分配速度** | 非常快（移动指针） | 较慢（需要查找合适的空闲块） |
| **释放方式** | 自动（函数返回时） | 手动（调用 free） |
| **大小限制** | 小（通常 8MB） | 大（受系统内存限制） |
| **碎片问题** | 无 | 可能产生内存碎片 |
| **访问速度** | 快（缓存友好） | 较慢（可能分散在不同位置） |
| **生命周期** | 函数作用域 | 手动管理 |
| **线程安全** | 每个线程独立栈 | 多线程共享，需同步 |
| **适用场景** | 小型临时数据 | 大型、动态大小、长生命周期数据 |

### 何时使用堆？
1. **大小在编译时未知**：动态数组、链表、树等数据结构
2. **数据量大**：超过栈大小限制的数据
3. **生命周期长**：需要在函数间传递，或者长时间保存
4. **需要手动控制生命周期**：精确控制内存分配和释放时机

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 创建一个动态大小的字符串数组
char** create_string_array(int count, int max_length) {
    // 分配指针数组
    char **arr = malloc(count * sizeof(char*));
    if (arr == NULL) return NULL;
    
    // 为每个字符串分配内存
    for (int i = 0; i < count; i++) {
        arr[i] = malloc(max_length * sizeof(char));
        if (arr[i] == NULL) {
            // 分配失败，释放已分配的内存
            for (int j = 0; j < i; j++) {
                free(arr[j]);
            }
            free(arr);
            return NULL;
        }
    }
    
    return arr;
}

// 释放字符串数组
void free_string_array(char **arr, int count) {
    if (arr == NULL) return;
    
    for (int i = 0; i < count; i++) {
        free(arr[i]);
    }
    free(arr);
}

int main() {
    int n = 10;
    char **strings = create_string_array(n, 100);
    
    if (strings != NULL) {
        // 使用字符串数组
        for (int i = 0; i < n; i++) {
            snprintf(strings[i], 100, "String %d", i);
            printf("%s\n", strings[i]);
        }
        
        // 释放内存
        free_string_array(strings, n);
    }
    
    return 0;
}
```

### 堆内存管理的底层原理
堆内存管理器（如 glibc 的 malloc 实现）维护一个空闲块链表：

```
堆内存结构示例：
┌───────────────┐
│  空闲块头部    │  包含大小信息和指向下一个空闲块的指针
├───────────────┤
│  空闲空间      │
├───────────────┤
│  已分配块头部  │
├───────────────┤
│  用户数据      │  malloc 返回的指针指向这里
├───────────────┤
│  已分配块头部  │
├───────────────┤
│  用户数据      │
└───────────────┘
```

分配策略：
- **首次适配（First Fit）**：找到第一个足够大的空闲块
- **最佳适配（Best Fit）**：找到大小最接近的空闲块
- **最坏适配（Worst Fit）**：找到最大的空闲块

### 堆内存的高级用法
```c
#include <stdio.h>
#include <stdlib.h>

// 使用柔性数组成员（Flexible Array Member）
struct data_packet {
    int size;
    int type;
    char data[];  // 柔性数组成员，不占用结构体大小
};

struct data_packet* create_packet(int data_size) {
    // 分配结构体 + 可变长度数据
    struct data_packet *packet = malloc(sizeof(struct data_packet) + data_size);
    if (packet) {
        packet->size = data_size;
        packet->type = 1;
    }
    return packet;
}

int main() {
    struct data_packet *pkt = create_packet(1024);
    if (pkt) {
        // 使用 packet->data[0..1023]
        free(pkt);
    }
    return 0;
}
```

## 内存泄漏（Memory Leak）

### 什么是内存泄漏？
内存泄漏是指程序分配的堆内存在不再使用后没有被释放，导致可用内存逐渐减少，最终可能导致系统内存耗尽。

### 常见的内存泄漏场景

#### 1. 忘记释放内存
```c
void memory_leak_example1() {
    int *data = malloc(1000 * sizeof(int));
    // 使用 data...
    // 忘记调用 free(data);
}  // 泄漏：1000 * 4 = 4000 字节
```

#### 2. 重新分配前未释放
```c
void memory_leak_example2() {
    int *ptr = malloc(100 * sizeof(int));
    
    // 危险：丢失了原来的指针，无法释放原内存
    ptr = malloc(200 * sizeof(int));
    
    free(ptr);  // 只释放了第二次分配的内存
}  // 泄漏：第一次分配的 400 字节
```

#### 3. 条件分支中的泄漏
```c
void memory_leak_example3(int condition) {
    char *buffer = malloc(1024);
    if (buffer == NULL) return;
    
    if (condition) {
        // 提前返回，忘记释放
        return;  // 泄漏！
    }
    
    // 正常路径释放
    free(buffer);
}
```

#### 4. 异常/错误处理不当
```c
int* allocate_and_process() {
    int *data = malloc(100 * sizeof(int));
    if (data == NULL) return NULL;
    
    int *temp = malloc(50 * sizeof(int));
    if (temp == NULL) {
        // 忘记释放 data
        return NULL;  // 泄漏！
    }
    
    // 处理数据...
    free(temp);
    return data;
}
```

#### 5. 循环中的泄漏
```c
void memory_leak_example5() {
    for (int i = 0; i < 1000; i++) {
        char *buffer = malloc(1024);
        // 使用 buffer...
        // 忘记释放
    }  // 泄漏：1000 * 1024 = 1MB
}
```

### 正确的内存管理模式

#### 模式 1：立即配对 malloc 和 free
```c
void good_pattern1() {
    int *data = malloc(100 * sizeof(int));
    if (data == NULL) {
        perror("malloc failed");
        return;
    }
    
    // 使用 data...
    
    free(data);
    data = NULL;  // 防止悬空指针
}
```

#### 模式 2：使用 goto 进行清理
```c
int good_pattern2() {
    int *data1 = NULL;
    int *data2 = NULL;
    int result = -1;
    
    data1 = malloc(100 * sizeof(int));
    if (data1 == NULL) goto cleanup;
    
    data2 = malloc(200 * sizeof(int));
    if (data2 == NULL) goto cleanup;
    
    // 处理数据...
    result = 0;
    
cleanup:
    free(data1);
    free(data2);
    return result;
}
```

#### 模式 3：封装资源管理
```c
typedef struct {
    void *data;
    size_t size;
} Buffer;

Buffer* buffer_create(size_t size) {
    Buffer *buf = malloc(sizeof(Buffer));
    if (buf == NULL) return NULL;
    
    buf->data = malloc(size);
    if (buf->data == NULL) {
        free(buf);
        return NULL;
    }
    
    buf->size = size;
    return buf;
}

void buffer_destroy(Buffer *buf) {
    if (buf) {
        free(buf->data);
        free(buf);
    }
}

void good_pattern3() {
    Buffer *buf = buffer_create(1024);
    if (buf) {
        // 使用 buf...
        buffer_destroy(buf);
    }
}
```

### 检测内存泄漏的工具

#### 1. Valgrind（最常用）
```bash
# 编译时加上调试信息
gcc -g -o program program.c

# 使用 Valgrind 检测内存泄漏
valgrind --leak-check=full --show-leak-kinds=all ./program

# 输出示例：
# ==12345== HEAP SUMMARY:
# ==12345==     in use at exit: 400 bytes in 1 blocks
# ==12345==   total heap usage: 1 allocs, 0 frees, 400 bytes allocated
# ==12345== 
# ==12345== 400 bytes in 1 blocks are definitely lost
```

#### 2. AddressSanitizer（编译器内置）
```bash
# 使用 GCC 或 Clang 编译
gcc -fsanitize=address -g -o program program.c

# 运行程序，自动检测内存问题
./program

# 输出会显示内存泄漏的位置和堆栈跟踪
```

#### 3. 自定义内存跟踪
```c
#include <stdio.h>
#include <stdlib.h>

// 简单的内存跟踪（调试用）
#ifdef DEBUG_MEMORY
static int alloc_count = 0;
static int free_count = 0;

void* debug_malloc(size_t size, const char *file, int line) {
    void *ptr = malloc(size);
    if (ptr) {
        alloc_count++;
        printf("[ALLOC] %p: %zu bytes at %s:%d (count=%d)\n", 
               ptr, size, file, line, alloc_count);
    }
    return ptr;
}

void debug_free(void *ptr, const char *file, int line) {
    if (ptr) {
        free_count++;
        printf("[FREE] %p at %s:%d (count=%d)\n", 
               ptr, file, line, free_count);
        free(ptr);
    }
}

void print_memory_stats() {
    printf("\n[MEMORY STATS]\n");
    printf("Allocations: %d\n", alloc_count);
    printf("Frees: %d\n", free_count);
    printf("Leaked: %d\n", alloc_count - free_count);
}

#define malloc(size) debug_malloc(size, __FILE__, __LINE__)
#define free(ptr) debug_free(ptr, __FILE__, __LINE__)
#endif

int main() {
    int *p1 = malloc(100);
    int *p2 = malloc(200);
    
    free(p1);
    // 忘记释放 p2
    
#ifdef DEBUG_MEMORY
    print_memory_stats();
#endif
    
    return 0;
}
```

编译运行：
```bash
gcc -DDEBUG_MEMORY -o program program.c
./program
```

### 内存泄漏的影响
1. **程序性能下降**：可用内存减少，导致频繁换页
2. **系统不稳定**：长期运行可能耗尽系统内存
3. **程序崩溃**：最终可能因为无法分配内存而失败
4. **影响其他程序**：占用过多系统资源

## 栈溢出（Stack Overflow）

### 什么是栈溢出？
栈溢出是指程序使用的栈空间超过了系统分配的限制，通常导致程序崩溃（Segmentation Fault）。

### 栈溢出的常见原因

#### 1. 递归过深
```c
#include <stdio.h>

// 危险：无限递归导致栈溢出
void infinite_recursion(int n) {
    printf("n = %d\n", n);
    infinite_recursion(n + 1);  // 没有终止条件
}

// 危险：递归深度太大
int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

int main() {
    // infinite_recursion(0);  // 几千次调用后栈溢出
    // fibonacci(50);           // 递归次数指数增长，栈溢出
    return 0;
}
```

#### 2. 局部数组过大
```c
void large_local_array() {
    // 危险：在栈上分配 10MB
    char buffer[10 * 1024 * 1024];  // 超过默认栈大小（通常 8MB）
    
    // 使用 buffer...
}  // 栈溢出！
```

#### 3. 大量局部变量
```c
void many_local_variables() {
    int array1[100000];
    int array2[100000];
    int array3[100000];
    // 总共约 1.2MB，可能导致栈溢出
}
```

#### 4. 嵌套函数调用过深
```c
void level1(int n) {
    char buffer[1000];
    if (n > 0) {
        level2(n - 1);
    }
}

void level2(int n) {
    char buffer[1000];
    if (n > 0) {
        level1(n - 1);
    }
}

int main() {
    level1(10000);  // 10000 层嵌套，每层 1KB，总共 10MB
    return 0;
}
```

### 检测栈溢出

#### 编译时警告
```bash
# GCC 可以检测某些栈溢出风险
gcc -Wstack-usage=8192 -o program program.c
# 警告：函数使用超过 8192 字节的栈空间
```

#### 运行时检测
```bash
# 使用 AddressSanitizer 检测栈溢出
gcc -fsanitize=address -g -o program program.c
./program

# 输出会显示栈溢出的详细信息：
# ASAN:DEADLYSIGNAL
# AddressSanitizer: stack-overflow on address 0x...
```

#### 手动检查
```c
#include <stdio.h>
#include <sys/resource.h>

void print_stack_limit() {
    struct rlimit limit;
    getrlimit(RLIMIT_STACK, &limit);
    
    printf("Current stack limit: ");
    if (limit.rlim_cur == RLIM_INFINITY) {
        printf("unlimited\n");
    } else {
        printf("%lu bytes (%lu MB)\n", 
               limit.rlim_cur, limit.rlim_cur / (1024 * 1024));
    }
}
```

### 解决栈溢出的方法

#### 方法 1：使用堆内存
```c
#include <stdlib.h>

// 错误：栈分配
void stack_allocation() {
    int large_array[1000000];  // 4MB，可能栈溢出
    // 使用 large_array...
}

// 正确：堆分配
void heap_allocation() {
    int *large_array = malloc(1000000 * sizeof(int));  // 4MB，在堆上
    if (large_array == NULL) {
        perror("malloc failed");
        return;
    }
    
    // 使用 large_array...
    
    free(large_array);
}
```

#### 方法 2：优化递归为迭代
```c
// 递归版本（可能栈溢出）
int factorial_recursive(int n) {
    if (n <= 1) return 1;
    return n * factorial_recursive(n - 1);
}

// 迭代版本（不会栈溢出）
int factorial_iterative(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}
```

#### 方法 3：使用尾递归优化
```c
// 尾递归（某些编译器可以优化为迭代）
int factorial_tail(int n, int accumulator) {
    if (n <= 1) return accumulator;
    return factorial_tail(n - 1, n * accumulator);
}

// 使用 -O2 或 -O3 编译，编译器可能优化为循环
// gcc -O2 -o program program.c
```

#### 方法 4：增加栈大小
```bash
# 临时增加栈大小（当前 shell）
ulimit -s 16384  # 设置为 16MB

# 在程序中设置
#include <sys/resource.h>

void increase_stack_size() {
    struct rlimit limit;
    limit.rlim_cur = 16 * 1024 * 1024;  // 16MB
    limit.rlim_max = 16 * 1024 * 1024;
    
    if (setrlimit(RLIMIT_STACK, &limit) != 0) {
        perror("setrlimit failed");
    }
}

int main() {
    increase_stack_size();
    // 继续执行...
    return 0;
}
```

#### 方法 5：使用动态内存池
```c
#include <stdlib.h>

typedef struct {
    void *data;
    size_t size;
} MemoryPool;

MemoryPool* pool_create(size_t size) {
    MemoryPool *pool = malloc(sizeof(MemoryPool));
    if (pool) {
        pool->data = malloc(size);
        pool->size = size;
        if (pool->data == NULL) {
            free(pool);
            return NULL;
        }
    }
    return pool;
}

void pool_destroy(MemoryPool *pool) {
    if (pool) {
        free(pool->data);
        free(pool);
    }
}

void use_memory_pool() {
    MemoryPool *pool = pool_create(10 * 1024 * 1024);  // 10MB
    if (pool) {
        // 使用 pool->data 作为工作空间
        pool_destroy(pool);
    }
}
```

### 实际案例

#### 案例 1：深度优先搜索优化
```c
#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 1000

// 递归版本（可能栈溢出）
void dfs_recursive(int graph[][MAX_SIZE], int visited[], int node, int n) {
    visited[node] = 1;
    printf("%d ", node);
    
    for (int i = 0; i < n; i++) {
        if (graph[node][i] && !visited[i]) {
            dfs_recursive(graph, visited, i, n);
        }
    }
}

// 栈模拟版本（使用堆，不会栈溢出）
void dfs_iterative(int graph[][MAX_SIZE], int visited[], int start, int n) {
    int *stack = malloc(n * sizeof(int));
    int top = -1;
    
    stack[++top] = start;
    
    while (top >= 0) {
        int node = stack[top--];
        
        if (visited[node]) continue;
        
        visited[node] = 1;
        printf("%d ", node);
        
        for (int i = n - 1; i >= 0; i--) {
            if (graph[node][i] && !visited[i]) {
                stack[++top] = i;
            }
        }
    }
    
    free(stack);
}
```

#### 案例 2：快速排序优化
```c
#include <stdlib.h>

// 递归版本（深度可能很大）
void quicksort_recursive(int arr[], int low, int high) {
    if (low < high) {
        int pivot = partition(arr, low, high);
        quicksort_recursive(arr, low, pivot - 1);
        quicksort_recursive(arr, pivot + 1, high);
    }
}

// 优化版本：小数组使用插入排序，大数组递归
#define THRESHOLD 10

void quicksort_optimized(int arr[], int low, int high) {
    if (high - low < THRESHOLD) {
        insertion_sort(arr, low, high);
    } else if (low < high) {
        int pivot = partition(arr, low, high);
        
        // 先递归较小的部分，减少栈深度
        if (pivot - low < high - pivot) {
            quicksort_optimized(arr, low, pivot - 1);
            quicksort_optimized(arr, pivot + 1, high);
        } else {
            quicksort_optimized(arr, pivot + 1, high);
            quicksort_optimized(arr, low, pivot - 1);
        }
    }
}
```

### 调试栈溢出
```bash
# 使用 GDB 调试
gdb ./program
(gdb) run
# 程序崩溃后
(gdb) backtrace    # 查看调用栈
(gdb) frame 0      # 检查每一帧
(gdb) info locals  # 查看局部变量

# 使用 core dump
ulimit -c unlimited  # 允许生成 core dump
./program
# 程序崩溃后生成 core 文件
gdb ./program core
(gdb) backtrace
```

## 其他常见内存问题

### 悬空指针（Dangling Pointer）
指针指向已经释放的内存。

```c
int* create_dangling_pointer() {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;
    free(ptr);
    return ptr;  // 悬空指针！
}

void use_after_free() {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;
    free(ptr);
    
    printf("%d\n", *ptr);  // 未定义行为：使用已释放的内存
    *ptr = 100;             // 未定义行为：写入已释放的内存
}

// 解决方法
void safe_free(int **ptr) {
    if (ptr && *ptr) {
        free(*ptr);
        *ptr = NULL;  // 设置为 NULL，避免悬空指针
    }
}

void correct_usage() {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;
    safe_free(&ptr);
    
    if (ptr != NULL) {  // 检查通过
        *ptr = 100;
    }
}
```

### 重复释放（Double Free）
对同一块内存释放两次。

```c
void double_free_error() {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;
    
    free(ptr);
    free(ptr);  // 错误：重复释放
}

// 解决方法
void safe_usage() {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;
    
    free(ptr);
    ptr = NULL;  // 设置为 NULL
    
    free(ptr);  // free(NULL) 是安全的，什么都不做
}
```

### 缓冲区溢出（Buffer Overflow）
写入数据超过分配的空间。

```c
void buffer_overflow_stack() {
    char buffer[10];
    // 危险：写入 20 个字符到只能容纳 10 个的缓冲区
    strcpy(buffer, "This is a long string");  // 栈溢出
}

void buffer_overflow_heap() {
    char *buffer = malloc(10);
    strcpy(buffer, "This is a long string");  // 堆溢出
    free(buffer);
}

// 解决方法：使用安全函数
#include <string.h>
void safe_string_copy() {
    char buffer[10];
    const char *source = "This is a long string";
    
    strncpy(buffer, source, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';  // 确保以 null 结尾
    
    // 或使用 snprintf
    snprintf(buffer, sizeof(buffer), "%s", source);
}
```

### 内存对齐（Memory Alignment）
CPU 访问对齐的数据更高效。

```c
#include <stdio.h>
#include <stddef.h>

struct unaligned {
    char a;      // 1 byte
    int b;       // 4 bytes
    char c;      // 1 byte
};  // 实际大小：12 字节（因为填充）

struct aligned {
    char a;      // 1 byte
    char c;      // 1 byte
    int b;       // 4 bytes
};  // 实际大小：8 字节

int main() {
    printf("sizeof(struct unaligned) = %zu\n", sizeof(struct unaligned));  // 12
    printf("sizeof(struct aligned) = %zu\n", sizeof(struct aligned));      // 8
    
    // 查看成员偏移
    printf("offsetof(unaligned, a) = %zu\n", offsetof(struct unaligned, a));
    printf("offsetof(unaligned, b) = %zu\n", offsetof(struct unaligned, b));
    printf("offsetof(unaligned, c) = %zu\n", offsetof(struct unaligned, c));
    
    return 0;
}
```

### 野指针（Wild Pointer）
未初始化的指针。

```c
void wild_pointer() {
    int *ptr;  // 未初始化，指向随机地址
    *ptr = 42; // 未定义行为！
}

// 解决方法
void initialized_pointer() {
    int *ptr = NULL;  // 初始化为 NULL
    
    ptr = malloc(sizeof(int));
    if (ptr != NULL) {
        *ptr = 42;
        free(ptr);
        ptr = NULL;
    }
}
```

## 内存布局实践示例

### 完整示例：展示所有内存区域
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 代码段：函数
void print_addresses();

// 数据段：已初始化的全局变量
int global_initialized = 100;
static int static_initialized = 200;
const int const_global = 300;

// BSS 段：未初始化的全局变量
int global_uninitialized;
static int static_uninitialized;

// 代码段：字符串常量
const char *string_literal = "Hello, World!";

int main(int argc, char *argv[]) {
    // 栈：局部变量
    int stack_var = 10;
    char stack_array[100];
    
    // 堆：动态分配
    int *heap_var = malloc(sizeof(int));
    *heap_var = 20;
    
    printf("=== 内存布局示例 ===\n\n");
    
    printf("代码段（Text Segment）：\n");
    printf("  函数 main:           %p\n", (void*)main);
    printf("  函数 print_addresses: %p\n", (void*)print_addresses);
    printf("  字符串常量:          %p (\"%s\")\n\n", (void*)string_literal, string_literal);
    
    printf("数据段（Data Segment）：\n");
    printf("  global_initialized:   %p (value=%d)\n", (void*)&global_initialized, global_initialized);
    printf("  static_initialized:   %p (value=%d)\n", (void*)&static_initialized, static_initialized);
    printf("  const_global:         %p (value=%d)\n\n", (void*)&const_global, const_global);
    
    printf("BSS 段：\n");
    printf("  global_uninitialized: %p (value=%d)\n", (void*)&global_uninitialized, global_uninitialized);
    printf("  static_uninitialized: %p (value=%d)\n\n", (void*)&static_uninitialized, static_uninitialized);
    
    printf("堆（Heap）：\n");
    printf("  heap_var:             %p (value=%d)\n\n", (void*)heap_var, *heap_var);
    
    printf("栈（Stack）：\n");
    printf("  stack_var:            %p (value=%d)\n", (void*)&stack_var, stack_var);
    printf("  stack_array:          %p\n", (void*)stack_array);
    printf("  argc:                 %p (value=%d)\n", (void*)&argc, argc);
    printf("  argv:                 %p\n\n", (void*)&argv);
    
    printf("命令行参数（高地址）：\n");
    for (int i = 0; i < argc; i++) {
        printf("  argv[%d]:             %p (\"%s\")\n", i, (void*)argv[i], argv[i]);
    }
    
    free(heap_var);
    return 0;
}

void print_addresses() {
    printf("This function is in the code segment.\n");
}
```

编译运行：
```bash
gcc -o memory_demo memory_demo.c
./memory_demo arg1 arg2
```

### 查看内存布局的工具和命令

#### 1. size 命令
```bash
# 查看各段大小
size program
# 输出：
#    text    data     bss     dec     hex filename
#    1234     560    1024    2818     b02 program
```

#### 2. readelf 命令
```bash
# 查看所有段
readelf -S program

# 查看特定段
readelf -S program | grep -E "\.text|\.data|\.bss"

# 查看程序头
readelf -l program
```

#### 3. objdump 命令
```bash
# 反汇编代码段
objdump -d program

# 查看所有段的内容
objdump -s program

# 查看符号表
objdump -t program
```

#### 4. nm 命令
```bash
# 查看符号及其位置
nm program

# 符号类型：
# T/t - 代码段（函数）
# D/d - 数据段（已初始化）
# B/b - BSS 段（未初始化）
# R/r - 只读数据段
```

#### 5. 查看运行时内存映射
```bash
# 运行程序
./program &
PID=$!

# 查看内存映射
cat /proc/$PID/maps

# 输出示例：
# 00400000-00401000 r-xp ... /path/to/program  （代码段）
# 00600000-00601000 r--p ... /path/to/program  （只读数据）
# 00601000-00602000 rw-p ... /path/to/program  （读写数据）
# 01234000-01256000 rw-p ... [heap]            （堆）
# 7fff12345000-7fff12366000 rw-p ... [stack]   （栈）
```

#### 6. 使用 GDB 调试器
```bash
gdb ./program
(gdb) break main
(gdb) run
(gdb) print &global_var      # 查看变量地址
(gdb) info variables         # 查看所有变量
(gdb) info functions         # 查看所有函数
(gdb) x/10i main             # 查看函数的机器码
```

## 内存管理最佳实践

### 1. 变量作用域和存储选择
```c
// 优先使用局部变量（栈）- 快速、自动管理
void process_data(int size) {
    if (size <= 1024) {
        // 小数组使用栈
        int buffer[1024];
        // 处理...
    } else {
        // 大数组使用堆
        int *buffer = malloc(size * sizeof(int));
        if (buffer) {
            // 处理...
            free(buffer);
        }
    }
}

// 静态局部变量 - 在函数间保持状态
int get_next_id() {
    static int counter = 0;  // 只初始化一次
    return ++counter;
}

// 避免全局变量 - 除非确实需要
// 不好：
int global_counter = 0;

// 更好：通过函数封装
static int counter = 0;  // 文件作用域，外部不可见
int get_counter() { return counter; }
void increment_counter() { counter++; }
```

### 2. 内存分配的通用模式
```c
// 分配-使用-释放模式
void* resource = allocate_resource();
if (resource == NULL) {
    handle_error();
    return;
}

use_resource(resource);

free_resource(resource);
resource = NULL;  // 防止悬空指针
```

### 3. RAII 风格的资源管理（C 中模拟）
```c
typedef struct {
    FILE *file;
    char *buffer;
} Resource;

Resource* resource_acquire(const char *filename, size_t buffer_size) {
    Resource *res = malloc(sizeof(Resource));
    if (!res) return NULL;
    
    res->file = fopen(filename, "r");
    if (!res->file) {
        free(res);
        return NULL;
    }
    
    res->buffer = malloc(buffer_size);
    if (!res->buffer) {
        fclose(res->file);
        free(res);
        return NULL;
    }
    
    return res;
}

void resource_release(Resource *res) {
    if (res) {
        if (res->file) fclose(res->file);
        if (res->buffer) free(res->buffer);
        free(res);
    }
}

void use_resource() {
    Resource *res = resource_acquire("data.txt", 1024);
    if (!res) return;
    
    // 使用资源...
    
    resource_release(res);
}
```

### 4. 内存安全检查清单
- [ ] 每个 `malloc`/`calloc`/`realloc` 都检查返回值
- [ ] 每个 `malloc` 都有对应的 `free`
- [ ] `free` 后将指针设置为 `NULL`
- [ ] 不返回局部变量的地址
- [ ] 使用 `strncpy`、`snprintf` 等安全函数
- [ ] 数组访问时检查边界
- [ ] 使用 Valgrind 或 AddressSanitizer 检测内存问题
- [ ] 避免在栈上分配大数组
- [ ] 静态分析工具定期检查代码

### 5. 性能优化建议
```c
// 1. 小对象使用栈，大对象使用堆
void optimize_small_objects() {
    struct Point { int x, y; };
    struct Point p;  // 栈上，快速
}

// 2. 重用内存，减少分配次数
void reuse_memory() {
    static char *buffer = NULL;
    static size_t buffer_size = 0;
    
    size_t needed = 1024;
    if (buffer_size < needed) {
        free(buffer);
        buffer = malloc(needed);
        buffer_size = needed;
    }
    // 使用 buffer...
}

// 3. 使用内存池
typedef struct MemoryPool {
    void *memory;
    size_t size;
    size_t used;
} MemoryPool;

void* pool_alloc(MemoryPool *pool, size_t size) {
    if (pool->used + size > pool->size) {
        return NULL;
    }
    void *ptr = (char*)pool->memory + pool->used;
    pool->used += size;
    return ptr;
}

// 4. 对齐优化
struct Aligned {
    int a;
    int b;
    char c;
    char d;
} __attribute__((aligned(16)));  // GCC 属性
```

## 总结

### 内存区域对比表
| 内存区域 | 大小 | 速度 | 生命周期 | 管理方式 | 适用场景 |
|---------|------|------|---------|---------|----------|
| **栈** | 小（MB级） | 很快 | 函数作用域 | 自动 | 小型临时数据 |
| **堆** | 大（GB级） | 较慢 | 手动管理 | 手动 | 大型/动态/长生命周期数据 |
| **数据段** | 固定 | 快 | 程序运行期 | 编译器 | 已初始化全局/静态变量 |
| **BSS段** | 固定 | 快 | 程序运行期 | 编译器 | 未初始化全局/静态变量 |
| **代码段** | 固定 | 快 | 程序运行期 | 编译器 | 程序指令和常量 |

### 关键要点
1. **栈**：快速、自动管理，但大小有限
2. **堆**：灵活、大容量，但需要手动管理
3. **数据段/BSS**：全局数据，整个程序生命周期
4. **代码段**：只读、可共享，存储程序指令
5. **内存安全**：防止泄漏、溢出、悬空指针
6. **工具使用**：Valgrind、AddressSanitizer、GDB
