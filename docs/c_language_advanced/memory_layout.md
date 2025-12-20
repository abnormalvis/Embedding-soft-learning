# 内存空间布局

## 进程内存分区概述

### 虚拟内存基础
在深入了解内存布局之前，需要理解一个关键概念：**虚拟内存**（Virtual Memory）。

每个进程看到的内存地址并不是物理内存的真实地址，而是虚拟地址。操作系统通过 **内存管理单元（MMU）** 将虚拟地址映射到物理地址。这样做有几个重要优势：

1. **进程隔离**：每个进程有独立的虚拟地址空间，一个进程无法访问另一个进程的内存
2. **内存保护**：可以设置不同内存区域的访问权限
3. **内存扩展**：虚拟内存可以超过物理内存大小（通过页面交换）
4. **灵活布局**：程序不需要关心物理内存的碎片化问题

```
虚拟地址到物理地址的映射：
进程 A:                     物理内存:
┌──────────┐               ┌──────────┐
│ 0x1000   │──────┐        │  ...     │
│ (虚拟)   │      │   ┌───>│ 0x5000   │ (物理)
└──────────┘      │   │    │  ...     │
                  └───┘    │          │
进程 B:                     │          │
┌──────────┐               │          │
│ 0x1000   │──────────────>│ 0x8000   │ (物理)
│ (虚拟)   │               │  ...     │
└──────────┘               └──────────┘

两个进程可以使用相同的虚拟地址，但映射到不同的物理地址
```

### 32 位 vs 64 位地址空间

| 特性 | 32 位系统 | 64 位系统 |
|------|-----------|-----------|
| 虚拟地址空间 | 4GB (2³²) | 理论上 16 EB (2⁶⁴)，实际 256 TB |
| 用户空间 | 约 3GB (Linux) 或 2GB (Windows) | 约 128 TB (Linux) |
| 内核空间 | 约 1GB (Linux) 或 2GB (Windows) | 约 128 TB (Linux) |
| 指针大小 | 4 字节 | 8 字节 |
| 单进程最大内存 | 受 4GB 限制 | 实际受物理内存和系统限制 |

示例代码查看地址空间：
```c
#include <stdio.h>
#include <stdint.h>

int main() {
    printf("指针大小: %zu 字节\n", sizeof(void*));
    printf("size_t 大小: %zu 字节\n", sizeof(size_t));
    printf("intptr_t 大小: %zu 字节\n", sizeof(intptr_t));
    
    #if defined(__x86_64__) || defined(_M_X64)
        printf("架构: 64 位\n");
    #elif defined(__i386) || defined(_M_IX86)
        printf("架构: 32 位\n");
    #endif
    
    return 0;
}
```

### C 程序运行时的完整内存布局

C 程序运行时的内存分为以下几个区域，每个区域都有其特定的用途和特性：

```
高地址（64 位: 0x00007FFFFFFFFFFF，32 位: 0xFFFFFFFF）
┌──────────────────┐
│   内核空间       │  仅内核可访问
│ (Kernel Space)   │  不在用户进程的虚拟地址空间中
├──────────────────┤ ← 用户空间与内核空间的分界线
│                  │
│  命令行参数和    │  环境变量（argv, envp）
│  环境变量        │  由操作系统传递，程序启动时初始化
│  (Arguments &    │  包含 main() 函数的 argc, argv 参数
│   Environment)   │  以及所有环境变量（如 PATH, HOME 等）
├──────────────────┤
│                  │
│      栈          │  ↓ 向下增长（从高地址到低地址）
│    (Stack)       │  • 函数调用栈帧（Stack Frame）
│                  │  • 局部变量（自动变量）
│  每个线程有      │  • 函数参数（实际是复制到栈上）
│  独立的栈        │  • 返回地址（函数调用完成后返回到哪里）
│                  │  • 保存的寄存器值（寄存器现场）
│  Linux 默认      │  • 栈帧指针（Frame Pointer, rbp/ebp）
│  8MB/线程        │  • 栈指针（Stack Pointer, rsp/esp）
│                  │  
│  优点：          │  特点：
│  - 速度极快      │  - LIFO（后进先出）数据结构
│  - 自动管理      │  - 编译时确定大小（对于固定数组）
│  - 缓存友好      │  - 函数返回时自动释放
│                  │  - 栈溢出检测（guard page）
├──────────────────┤
│       ↕          │  
│   未分配空间     │  栈和堆之间的"空洞"
│   (Unmapped)     │  可以随着栈和堆的增长而缩小
│       ↕          │  访问这片区域会触发段错误（Segmentation Fault）
│                  │  这是一个安全特性，防止栈和堆相撞
├──────────────────┤
│                  │
│      堆          │  ↑ 向上增长（从低地址到高地址）
│    (Heap)        │  • malloc/calloc/realloc 分配的内存
│                  │  • new 操作符分配的对象（C++）
│  所有线程共享    │  • 动态数据结构（链表、树、图等）
│                  │  • 运行时才能确定大小的数据
│  理论上可以      │  • 需要手动管理（malloc/free）
│  很大（GB级）    │  
│                  │  特点：
│  优点：          │  - 通过 brk() 或 mmap() 系统调用扩展
│  - 大小灵活      │  - 可能产生内存碎片
│  - 生命周期长    │  - 分配和释放相对较慢
│  - 可跨函数使用  │  - 需要内存分配器管理（如 ptmalloc）
├──────────────────┤
│                  │
│   BSS 段         │  未初始化数据段（Block Started by Symbol）
│   (.bss)         │  • 未初始化的全局变量
│                  │  • 未初始化的静态变量（全局和局部）
│  只读（初始化后  │  • 显式初始化为 0 的全局/静态变量
│  变为可读写）    │  
│                  │  示例：
│  不占用磁盘      │  int global_var;              // BSS
│  空间（可执行    │  static int static_var;       // BSS
│  文件中）        │  static int zero_var = 0;     // BSS
│                  │  
│                  │  特点：
│                  │  - 程序加载时由内核清零（memset 0）
│                  │  - 节省可执行文件大小
│                  │  - 对大型数组特别有用
├──────────────────┤
│                  │
│   数据段         │  已初始化数据段（Data Segment）
│   (.data)        │  • 已初始化的全局变量（非零值）
│                  │  • 已初始化的静态变量（非零值）
│  可读写          │  
│  (Read-Write)    │  示例：
│                  │  int global = 42;             // .data
│  占用磁盘空间    │  static int static_var = 100; // .data
│  （存储在可      │  
│  执行文件中）    │  细分：
│                  │  - .data 段：可读写数据
│                  │  - .rodata 段：只读数据（const 全局变量）
├──────────────────┤
│                  │
│   代码段         │  文本段（Text Segment）
│   (.text)        │  • 程序的机器指令（编译后的二进制代码）
│                  │  • 所有函数的实现代码
│  只读+可执行     │  • CPU 执行的指令序列
│  (Read-Only +    │  
│   Executable)    │  也可能包含：
│                  │  • 字符串字面量（如 "Hello"）
│  可以被多个      │  • const 全局常量（某些编译器）
│  进程共享        │  • 跳转表（switch 语句）
│  （节省物理      │  • 虚函数表（C++）
│   内存）         │  
│                  │  特点：
│                  │  - 程序运行期间大小固定
│                  │  - 写入会导致段错误（Segmentation Fault）
│                  │  - 可以被多个进程实例共享
│                  │  - 受到 DEP/NX 保护（数据执行保护）
└──────────────────┘
低地址（0x0000000000000000）

注意：实际地址布局受到以下因素影响：
- ASLR（地址空间布局随机化）：安全特性，随机化各段起始地址
- PIE（位置无关可执行文件）：代码段地址也会随机化
- 编译器和链接器设置
- 操作系统版本和配置
```

### 内存布局的重要特性

#### 1. 地址空间隔离（Process Isolation）
每个进程都有独立的虚拟地址空间，这意味着：
- 进程 A 的地址 0x1000 和进程 B 的地址 0x1000 指向不同的物理内存
- 一个进程无法直接访问另一个进程的内存（除非通过 IPC 机制）
- 进程崩溃不会影响其他进程

```c
// 示例：两个进程中的相同地址
// 进程 1
int *ptr = malloc(sizeof(int));
printf("进程 1: ptr = %p\n", ptr);  // 可能输出 0x55555576b2a0

// 进程 2（独立运行）
int *ptr = malloc(sizeof(int));
printf("进程 2: ptr = %p\n", ptr);  // 可能输出 0x55555576b2a0（相同虚拟地址！）

// 但它们指向不同的物理内存位置
```

#### 2. 内存保护（Memory Protection）
不同区域有不同的访问权限，由 MMU 强制执行：

| 区域 | 读（R） | 写（W） | 执行（X） | 权限标记 |
|------|---------|---------|-----------|----------|
| 代码段 | ✓ | ✗ | ✓ | r-x |
| 只读数据段 | ✓ | ✗ | ✗ | r-- |
| 数据段 | ✓ | ✓ | ✗ | rw- |
| BSS 段 | ✓ | ✓ | ✗ | rw- |
| 堆 | ✓ | ✓ | ✗ | rw- |
| 栈 | ✓ | ✓ | ✗ | rw- |

```c
// 违反内存保护的示例
void demonstrate_memory_protection() {
    // 1. 尝试修改代码段（字符串字面量）
    char *str = "Hello";  // 字符串在代码段（只读）
    // str[0] = 'h';      // 段错误！只读内存不能写入
    
    // 2. 正确方式：将字符串复制到可写内存
    char writable_str[] = "Hello";  // 在栈上，可写
    writable_str[0] = 'h';           // OK
    
    // 3. 尝试执行数据段的内容
    char code[] = {0x90, 0xC3};  // NOP + RET 指令
    void (*func)() = (void(*)())code;
    // func();  // 可能段错误！数据段不可执行（DEP/NX 保护）
}
```

#### 3. 动态增长（Dynamic Growth）
栈和堆可以在运行时动态增长：

**栈的增长：**
- 每次函数调用，栈自动向下增长
- 达到栈限制时，触发栈溢出（Stack Overflow）
- 现代系统使用 guard page 检测栈溢出

```c
void show_stack_growth(int depth) {
    int local_var;
    printf("深度 %d，局部变量地址: %p\n", depth, (void*)&local_var);
    
    if (depth < 5) {
        show_stack_growth(depth + 1);  // 递归调用，栈增长
    }
}

int main() {
    show_stack_growth(0);
    // 输出会显示地址递减（栈向下增长）
    return 0;
}
```

**堆的增长：**
- 通过 `brk()` 或 `sbrk()` 系统调用扩展堆空间
- 对于大块内存，使用 `mmap()` 创建匿名内存映射
- 堆可以有"洞"（已释放的内存块）

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void show_heap_growth() {
    printf("初始 program break: %p\n", sbrk(0));
    
    void *p1 = malloc(1024);
    printf("malloc 1KB 后: %p\n", sbrk(0));
    
    void *p2 = malloc(1024 * 1024);  // 1MB
    printf("malloc 1MB 后: %p\n", sbrk(0));
    
    free(p1);
    free(p2);
}
```

#### 4. 地址随机化（ASLR - Address Space Layout Randomization）
现代操作系统的安全特性，随机化内存布局以防止攻击：

```c
// 运行多次，观察地址变化
#include <stdio.h>
#include <stdlib.h>

int global_var = 42;

int main() {
    int stack_var = 10;
    int *heap_var = malloc(sizeof(int));
    
    printf("代码段（main 函数）:     %p\n", (void*)main);
    printf("数据段（全局变量）:     %p\n", (void*)&global_var);
    printf("栈（局部变量）:         %p\n", (void*)&stack_var);
    printf("堆（malloc 分配）:      %p\n", (void*)heap_var);
    
    free(heap_var);
    return 0;
}

// 多次运行，地址都不同（ASLR 生效）
// $ ./program
// 代码段（main 函数）:     0x5555555551a9
// $ ./program
// 代码段（main 函数）:     0x5555555561a9  （地址不同！）
```

查看和控制 ASLR：
```bash
# 查看 ASLR 状态（Linux）
cat /proc/sys/kernel/randomize_va_space
# 0 = 关闭, 1 = 部分随机化, 2 = 完全随机化

# 临时关闭 ASLR（调试用，需要 root 权限）
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space

# 为单个程序禁用 ASLR
setarch $(uname -m) -R ./program
```

#### 5. 内存页面（Memory Pages）
内存以页（Page）为单位管理，典型大小为 4KB：

```c
#include <stdio.h>
#include <unistd.h>

int main() {
    long page_size = sysconf(_SC_PAGESIZE);
    printf("系统页面大小: %ld 字节\n", page_size);
    
    long num_pages = sysconf(_SC_PHYS_PAGES);
    printf("物理页面数量: %ld\n", num_pages);
    
    long avail_pages = sysconf(_SC_AVPHYS_PAGES);
    printf("可用页面数量: %ld\n", avail_pages);
    
    printf("总物理内存: %.2f GB\n", 
           (double)(num_pages * page_size) / (1024*1024*1024));
    
    return 0;
}
```

#### 6. 内存映射文件（Memory-Mapped Files）
可以将文件映射到内存地址空间：

```c
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

void memory_map_example() {
    int fd = open("data.txt", O_RDONLY);
    if (fd == -1) return;
    
    size_t file_size = lseek(fd, 0, SEEK_END);
    
    // 将文件映射到内存
    char *mapped = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        close(fd);
        return;
    }
    
    // 现在可以像访问数组一样访问文件内容
    printf("文件第一个字符: %c\n", mapped[0]);
    
    munmap(mapped, file_size);
    close(fd);
}
```

## 代码段（Text Segment）

### 特性详解
- **存储内容**：程序的机器指令（编译后的二进制代码）
- **访问权限**：只读（Read-Only），可执行（Executable），权限标记为 `r-x`
- **共享性**：多个进程可以共享同一个程序的代码段，节省物理内存
- **大小固定**：在程序编译和链接时确定，运行时不可改变
- **位置**：通常位于虚拟内存的低地址区域（在某些系统上）
- **对齐**：通常对齐到页边界（4KB），便于内存管理
- **缓存优化**：CPU 指令缓存（I-Cache）专门缓存代码段

### 代码段的组成部分

#### 1. 函数机器指令
所有函数（包括 main、库函数）的编译后的机器码：

```c
#include <stdio.h>

// 这个函数编译后会变成机器指令存储在代码段
int add(int a, int b) {
    return a + b;
}

int main() {
    int result = add(3, 4);
    printf("Result: %d\n", result);
    return 0;
}
```

查看编译后的汇编代码：
```bash
# 编译并生成汇编代码
gcc -S -O0 program.c -o program.s

# 或者查看反汇编
gcc -c program.c -o program.o
objdump -d program.o

# 输出示例（x86-64）：
# 0000000000000000 <add>:
#    0:   55                      push   %rbp
#    1:   48 89 e5                mov    %rsp,%rbp
#    4:   89 7d fc                mov    %edi,-0x4(%rbp)
#    7:   89 75 f8                mov    %esi,-0x8(%rbp)
#    a:   8b 55 fc                mov    -0x4(%rbp),%edx
#    d:   8b 45 f8                mov    -0x8(%rbp),%eax
#   10:   01 d0                   add    %edx,%eax
#   12:   5d                      pop    %rbp
#   13:   c3                      retq
```

#### 2. 字符串字面量（String Literals）
使用双引号定义的字符串存储在代码段的只读数据区（.rodata）：

```c
#include <stdio.h>

int main() {
    // 这些字符串字面量存储在代码段（只读）
    const char *str1 = "Hello, World!";
    const char *str2 = "Hello, World!";
    
    // 编译器优化：相同的字符串可能共享同一份内存
    printf("str1 地址: %p\n", (void*)str1);
    printf("str2 地址: %p\n", (void*)str2);
    printf("地址相同吗？ %s\n", (str1 == str2) ? "是" : "否");
    
    // 字符数组在栈上，可以修改
    char arr1[] = "Hello, World!";
    char arr2[] = "Hello, World!";
    
    printf("arr1 地址: %p\n", (void*)arr1);
    printf("arr2 地址: %p\n", (void*)arr2);
    printf("地址相同吗？ %s\n", (arr1 == arr2) ? "是" : "否");  // 总是 "否"
    
    // 可以修改数组内容
    arr1[0] = 'h';  // OK
    // str1[0] = 'h';  // 段错误！不能修改代码段
    
    return 0;
}
```

#### 3. 常量数据
包括 const 全局变量、枚举常量、某些字面量：

```c
// const 全局变量可能在 .rodata 段
const int MAX_SIZE = 100;
const double PI = 3.14159265359;
const char *ERROR_MSG = "An error occurred";

// 数组常量
const int lookup_table[] = {1, 2, 4, 8, 16, 32, 64, 128};

int main() {
    // MAX_SIZE = 200;  // 编译错误：不能修改 const 变量
    
    printf("MAX_SIZE 地址: %p\n", (void*)&MAX_SIZE);
    printf("PI 地址: %p\n", (void*)&PI);
    printf("lookup_table 地址: %p\n", (void*)lookup_table);
    
    return 0;
}
```

#### 4. 跳转表（Jump Tables）
用于优化 switch 语句：

```c
#include <stdio.h>

void demonstrate_jump_table(int value) {
    // 对于连续的 case 值，编译器可能生成跳转表
    switch (value) {
        case 0:  printf("Zero\n");  break;
        case 1:  printf("One\n");   break;
        case 2:  printf("Two\n");   break;
        case 3:  printf("Three\n"); break;
        case 4:  printf("Four\n");  break;
        case 5:  printf("Five\n");  break;
        default: printf("Other\n"); break;
    }
}

int main() {
    demonstrate_jump_table(3);
    return 0;
}
```

查看生成的跳转表：
```bash
gcc -O2 -c program.c -o program.o
objdump -d program.o
# 会看到类似的跳转表结构
```

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

#### 1. 安全性（Security）
防止程序意外或恶意修改自己的代码。如果代码段可写，恶意代码可以：
- 修改程序逻辑，绕过安全检查
- 注入恶意指令（代码注入攻击）
- 破坏程序的正常执行流程

```c
// 假设代码段可写（实际上不行）
void hypothetical_attack() {
    // 尝试修改 main 函数的第一条指令
    unsigned char *main_ptr = (unsigned char*)main;
    // main_ptr[0] = 0xC3;  // RET 指令，立即返回
    // 这会导致程序一启动就退出
}
```

#### 2. 共享性（Sharing）
多个进程可以共享同一份代码，极大节省物理内存：

```
假设 10 个用户同时运行 Firefox：
- 代码段大小：约 200MB
- 如果可写：需要 200MB × 10 = 2GB 物理内存
- 只读共享：只需要 200MB 物理内存（共享）+ 每个进程的数据段

节省：1.8GB 物理内存！
```

演示代码共享：
```c
// 编译这个程序
// gcc -o shared_demo shared_demo.c

#include <stdio.h>
#include <unistd.h>

int main() {
    printf("进程 PID: %d\n", getpid());
    printf("main 函数地址: %p\n", (void*)main);
    
    // 保持运行，以便检查内存映射
    printf("按 Enter 继续...\n");
    getchar();
    
    return 0;
}
```

在另一个终端运行：
```bash
# 启动两个实例
./shared_demo &
./shared_demo &

# 查看内存映射（代码段共享）
ps aux | grep shared_demo
cat /proc/<PID1>/maps | grep shared_demo
cat /proc/<PID2>/maps | grep shared_demo
# 代码段的物理地址相同！
```

#### 3. 稳定性（Stability）
避免程序bug导致代码被破坏：

```c
// 常见的缓冲区溢出错误
void vulnerable_function() {
    char buffer[10];
    // 如果这里发生溢出，而代码段在附近...
    strcpy(buffer, "This is a very long string that overflows");
    // 如果代码段可写，可能会覆盖附近的函数代码
}
```

#### 4. 调试和诊断（Debugging）
只读特性帮助快速定位问题：
- 修改代码段会立即触发段错误
- 明确指示程序存在严重bug
- 防止问题扩散到其他部分

### 代码段与数据执行保护（DEP/NX）

现代 CPU 支持 NX（No-Execute）位或 DEP（Data Execution Prevention）：
- 标记数据段、栈、堆为不可执行
- 防止缓冲区溢出攻击中注入的代码被执行
- 与只读代码段配合，提供双重保护

```c
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

void demonstrate_nx_protection() {
    // 尝试执行栈上的代码（会失败）
    unsigned char shellcode[] = {
        0x90,  // NOP
        0xC3   // RET
    };
    
    void (*func)() = (void(*)())shellcode;
    printf("尝试执行栈上的代码...\n");
    // func();  // 段错误！栈不可执行（NX 保护）
    
    // 正确方式：使用 mmap 分配可执行内存
    void *exec_mem = mmap(NULL, 4096, 
                          PROT_READ | PROT_WRITE | PROT_EXEC,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (exec_mem != MAP_FAILED) {
        memcpy(exec_mem, shellcode, sizeof(shellcode));
        void (*exec_func)() = (void(*)())exec_mem;
        exec_func();  // 这次可以执行
        munmap(exec_mem, 4096);
    }
}
```

检查 NX 保护：
```bash
# 检查程序是否启用 NX/DEP
readelf -l program | grep -i stack
# 如果看到 GNU_STACK 且权限是 RW（而不是 RWE），则 NX 启用

# 或使用 checksec
checksec --file=program
```

### 位置无关代码（PIC - Position Independent Code）

为了支持 ASLR 和共享库，代码需要是位置无关的：

```c
// pic_demo.c
#include <stdio.h>

int global_var = 42;

int get_value() {
    return global_var;  // 访问全局变量
}

int main() {
    printf("Value: %d\n", get_value());
    printf("global_var 地址: %p\n", (void*)&global_var);
    printf("get_value 地址: %p\n", (void*)get_value);
    return 0;
}
```

编译为 PIC：
```bash
# 非 PIC（默认）
gcc -o non_pic pic_demo.c

# PIC（用于共享库或 PIE 可执行文件）
gcc -fPIC -o pic pic_demo.c

# PIE（位置无关可执行文件）
gcc -fPIE -pie -o pie pic_demo.c

# 查看差异
readelf -h non_pic | grep Type
readelf -h pie | grep Type

# 多次运行，PIE 版本地址会变化（ASLR）
./pie
./pie
```

PIC 代码的特点：
- 使用相对寻址而不是绝对地址
- 通过 GOT（Global Offset Table）访问全局数据
- 通过 PLT（Procedure Linkage Table）调用外部函数
- 稍微降低性能（额外的间接寻址），但提高安全性

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

每次函数调用都会创建一个栈帧（也叫活动记录），包含该函数执行所需的所有信息。

#### 栈帧的完整结构

```
栈帧结构（从高地址到低地址，x86-64 架构）：

高地址
┌─────────────────────┐
│  参数 N             │  ← 如果参数超过 6 个（x86-64）
│  ...                │     前 6 个通过寄存器传递
│  参数 7             │     多余的参数压入栈
├─────────────────────┤
│  返回地址           │  ← call 指令自动压入
│  (Return Address)   │     存储调用后的下一条指令地址
│                     │     函数返回时，pop 到 IP 寄存器
├─────────────────────┤
│  旧的帧指针 (RBP)   │  ← push %rbp
│  (Saved Frame Ptr)  │     保存调用者的栈帧基址
├─────────────────────┤  ← 当前 RBP（帧指针）指向这里
│                     │     mov %rsp, %rbp
│  局部变量 1         │  ← sub $N, %rsp 分配空间
│  local1             │  
├─────────────────────┤     访问方式：
│  局部变量 2         │     local1: -8(%rbp)
│  buffer[100]        │     buffer: -108(%rbp)
│  (数组)             │     local2: -112(%rbp)
├─────────────────────┤
│  局部变量 3         │  
│  local2             │  
├─────────────────────┤
│  保存的寄存器       │  ← 如果函数需要使用被调用者
│  (Callee-saved)     │     保存寄存器（RBX, R12-R15）
│  如 RBX, R12        │     必须在函数开始时保存
├─────────────────────┤     在函数返回前恢复
│  临时变量空间       │  
│  (Temporary Space)  │  ← 用于表达式求值
├─────────────────────┤     复杂计算的中间结果
│  对齐填充           │  
│  (Padding)          │  ← 保持栈 16 字节对齐（x86-64 ABI）
└─────────────────────┘  ← 当前 RSP（栈指针）指向这里
低地址

关键寄存器（x86-64）：
- RSP（Stack Pointer）: 始终指向栈顶（当前可用位置）
- RBP（Base/Frame Pointer）: 指向当前栈帧的基址
- RIP（Instruction Pointer）: 指向当前执行的指令

32 位系统（x86）使用 ESP、EBP、EIP
```

#### 详细示例代码
```c
#include <stdio.h>

void show_addresses(int param1, int param2) {
    int local1 = 10;
    char buffer[100];
    int local2 = 20;
    
    printf("\n=== 栈帧地址分析 ===\n");
    
    // 参数地址（可能在栈上或寄存器中）
    printf("param1 地址: %p\n", (void*)&param1);
    printf("param2 地址: %p\n", (void*)&param2);
    
    // 局部变量地址（从高到低）
    printf("local1 地址: %p\n", (void*)&local1);
    printf("buffer 地址: %p\n", (void*)buffer);
    printf("local2 地址: %p\n", (void*)&local2);
    
    // 计算地址差（观察内存布局）
    printf("\nlocal1 到 buffer: %ld 字节\n", 
           (char*)&local1 - (char*)buffer);
    printf("buffer 到 local2: %ld 字节\n", 
           (char*)buffer - (char*)&local2);
    
    // 尝试访问返回地址（危险！仅用于演示）
    void **frame_ptr = (void**)__builtin_frame_address(0);
    printf("\n当前帧指针: %p\n", frame_ptr);
    printf("返回地址: %p\n", *(frame_ptr + 1));
}

int main() {
    printf("调用 show_addresses()...\n");
    show_addresses(100, 200);
    return 0;
}
```

#### 栈帧的创建和销毁过程

**函数调用时（Prologue - 序言）：**
```assembly
; 调用者（Caller）的操作：
call show_addresses    ; 1. 压入返回地址到栈
                      ; 2. 跳转到函数入口

; 被调用者（Callee）的操作：
show_addresses:
    push %rbp          ; 3. 保存旧的帧指针
    mov %rsp, %rbp     ; 4. 设置新的帧指针
    sub $128, %rsp     ; 5. 为局部变量分配空间
    
    ; 如果需要，保存被调用者保存寄存器
    push %rbx
    push %r12
    
    ; 函数体代码...
```

**函数返回时（Epilogue - 尾声）：**
```assembly
    ; 恢复被调用者保存寄存器
    pop %r12
    pop %rbx
    
    ; 恢复栈和帧指针
    mov %rbp, %rsp     ; 6. 恢复栈指针
    pop %rbp           ; 7. 恢复旧的帧指针
    ret                ; 8. 弹出返回地址并跳转
```

#### 实际查看栈帧

使用 GDB 调试器：
```bash
# 编译时添加调试信息
gcc -g -O0 -o stack_demo stack_demo.c

# 使用 GDB
gdb ./stack_demo

(gdb) break show_addresses
(gdb) run
(gdb) info frame          # 查看当前栈帧信息
(gdb) info locals         # 查看局部变量
(gdb) info args           # 查看参数
(gdb) backtrace           # 查看调用栈
(gdb) x/20xg $rsp         # 查看栈内存（16进制）
(gdb) x/20xg $rbp         # 从帧指针开始查看
```

输出示例：
```
Stack level 0, frame at 0x7fffffffe000:
 rip = 0x555555555189 in show_addresses; saved rip = 0x5555555551c5
 called by frame at 0x7fffffffe020
 Arglist at 0x7fffffffdf88, args: param1=100, param2=200
 Locals at 0x7fffffffdf88, Previous frame's sp is 0x7fffffffe000
 Saved registers:
  rbp at 0x7fffffffdf88, rip at 0x7fffffffdf90
```

#### 栈帧与函数调用约定

不同架构和编译器有不同的调用约定（Calling Convention）：

| 调用约定 | 参数传递 | 栈清理 | 适用 |
|----------|---------|--------|------|
| **cdecl** | 栈（从右到左） | 调用者 | x86 C 默认 |
| **stdcall** | 栈（从右到左） | 被调用者 | Win32 API |
| **fastcall** | 寄存器 + 栈 | 被调用者 | 性能优化 |
| **System V AMD64** | 前6个用寄存器 | 调用者 | x86-64 Linux |
| **MS x64** | 前4个用寄存器 | 调用者 | x86-64 Windows |

示例：
```c
// System V AMD64 调用约定（Linux x86-64）
void func(int a, int b, int c, int d, int e, int f, int g, int h) {
    // 参数传递方式：
    // a -> RDI 寄存器
    // b -> RSI 寄存器  
    // c -> RDX 寄存器
    // d -> RCX 寄存器
    // e -> R8 寄存器
    // f -> R9 寄存器
    // g -> 栈（第 1 个栈参数）
    // h -> 栈（第 2 个栈参数）
}
```

查看实际的调用约定：
```bash
gcc -S -masm=intel program.c -o program.s
cat program.s
# 查看参数如何传递
```

#### 栈帧链（Frame Chain）

多层函数调用形成栈帧链：

```c
#include <stdio.h>

void level3() {
    int var3 = 3;
    printf("Level 3, var3 at: %p\n", &var3);
    
    // 遍历栈帧链
    void **fp = (void**)__builtin_frame_address(0);
    printf("Frame 0 (level3): %p\n", fp);
    
    fp = (void**)*fp;  // 跟随帧指针链
    printf("Frame 1 (level2): %p\n", fp);
    
    fp = (void**)*fp;
    printf("Frame 2 (level1): %p\n", fp);
    
    fp = (void**)*fp;
    printf("Frame 3 (main): %p\n", fp);
}

void level2() {
    int var2 = 2;
    printf("Level 2, var2 at: %p\n", &var2);
    level3();
}

void level1() {
    int var1 = 1;
    printf("Level 1, var1 at: %p\n", &var1);
    level2();
}

int main() {
    int var0 = 0;
    printf("Main, var0 at: %p\n", &var0);
    level1();
    return 0;
}
```

运行结果会显示：
```
Main, var0 at: 0x7fffffffe00c
Level 1, var1 at: 0x7fffffffdfec
Level 2, var2 at: 0x7fffffffdfc c
Level 3, var3 at: 0x7fffffffdfac
Frame 0 (level3): 0x7fffffffdfa0
Frame 1 (level2): 0x7fffffffdfc0
Frame 2 (level1): 0x7fffffffdfe0
Frame 3 (main): 0x7fffffffe000
```

观察：地址递减（栈向下增长），帧指针形成链表。

#### 栈帧优化：帧指针省略（FPO）

编译器优化可以省略帧指针，节省寄存器：

```c
// 不使用 -fomit-frame-pointer
void func_with_fp(int x) {
    int local = x + 1;
    printf("%d\n", local);
}

// 使用 -fomit-frame-pointer
void func_without_fp(int x) {
    int local = x + 1;
    printf("%d\n", local);
}
```

编译对比：
```bash
# 保留帧指针
gcc -S -O2 -o with_fp.s func.c

# 省略帧指针（优化）
gcc -S -O2 -fomit-frame-pointer -o without_fp.s func.c

# 对比汇编代码
diff with_fp.s without_fp.s
```

省略帧指针的影响：
- ✅ 节省一个寄存器（RBP）
- ✅ 减少函数序言/尾声指令
- ✅ 提高性能（约 1-5%）
- ❌ 调试困难（无法轻易遍历栈帧）
- ❌ 某些性能分析工具需要帧指针

#### 栈帧与异常处理

栈帧信息对异常处理至关重要：

```c
#include <stdio.h>
#include <setjmp.h>

jmp_buf jump_buffer;

void risky_function() {
    printf("执行危险操作...\n");
    // 模拟异常
    longjmp(jump_buffer, 1);
    printf("这行不会执行\n");
}

void intermediate() {
    int local = 42;
    printf("Intermediate: local = %d at %p\n", local, &local);
    risky_function();
}

int main() {
    if (setjmp(jump_buffer) == 0) {
        printf("设置跳转点\n");
        intermediate();
    } else {
        printf("捕获到异常，栈已展开\n");
    }
    return 0;
}
```

现代 C++ 异常处理使用栈展开（Stack Unwinding）：
- 遍历栈帧链
- 调用析构函数
- 恢复寄存器状态
- 跳转到异常处理代码

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

#### 堆内存分配器的实现

堆内存管理器（如 glibc 的 ptmalloc2）需要解决几个关键问题：
1. 如何跟踪空闲和已分配的内存块
2. 如何快速找到合适大小的空闲块
3. 如何减少内存碎片
4. 如何处理并发访问（多线程）

#### 内存块结构

每个分配的内存块都有一个隐藏的头部（metadata）：

```c
// 简化的内存块结构
typedef struct malloc_chunk {
    size_t prev_size;   // 前一个块的大小（如果前一个块空闲）
    size_t size;        // 当前块的大小（包含头部）
                       // 最低 3 位用作标志位：
                       // - bit 0 (P): 前一个块是否在使用中
                       // - bit 1 (M): 是否通过 mmap 分配
                       // - bit 2 (A): 是否属于主arena
    
    // 如果块空闲，以下字段存储链表指针：
    struct malloc_chunk *fd;  // forward: 指向下一个空闲块
    struct malloc_chunk *bk;  // backward: 指向上一个空闲块
} malloc_chunk;
```

内存布局示意图：

```
已分配块的内存布局：
┌─────────────────────┐  ← chunk 地址
│  prev_size          │  4/8 字节（仅当前一块空闲时有效）
├─────────────────────┤
│  size | flags       │  4/8 字节
├─────────────────────┤  ← malloc() 返回的地址
│                     │
│  用户数据           │  用户请求的大小
│  (user data)        │
│                     │
├─────────────────────┤
│  未使用空间         │  对齐填充
└─────────────────────┘

空闲块的内存布局：
┌─────────────────────┐
│  prev_size          │
├─────────────────────┤
│  size | flags       │
├─────────────────────┤
│  fd（下一个空闲块） │  指针
├─────────────────────┤
│  bk（上一个空闲块） │  指针
├─────────────────────┤
│  未使用空间         │
├─────────────────────┤
│  size（块尾）       │  用于合并
└─────────────────────┘
```

#### 空闲块管理 - Bins

ptmalloc2 使用多个"bins"（桶）来管理不同大小的空闲块：

```
Bins 结构：

1. Fast Bins（快速分配箱，10 个）：
   - 小块内存（16-80 字节，64位系统）
   - 单向链表（LIFO）
   - 不合并相邻块（快速）
   - 用于频繁分配/释放的小对象
   
   [16] -> chunk -> chunk -> NULL
   [24] -> chunk -> NULL
   [32] -> chunk -> chunk -> chunk -> NULL
   ...

2. Small Bins（小型箱，62 个）：
   - 小到中等大小（<512/1024 字节）
   - 每个 bin 只包含固定大小的块
   - 双向链表（FIFO，减少碎片）
   - 相邻空闲块会合并
   
   [size 16]  <-> chunk <-> chunk <-> ...
   [size 24]  <-> chunk <-> chunk <-> ...
   ...

3. Large Bins（大型箱，63 个）：
   - 大块内存（>=512/1024 字节）
   - 每个 bin 包含一个范围的大小
   - 双向链表，按大小排序
   - 使用跳表加速查找
   
   [512-576)  <-> chunk(560) <-> chunk(520) <-> ...
   [576-640)  <-> chunk(600) <-> ...
   ...

4. Unsorted Bin（未排序箱，1 个）：
   - 临时存储最近释放的块
   - 下次分配时会重新整理到其他 bins
   - 用于优化频繁分配/释放的场景
   
   chunk <-> chunk <-> chunk <-> ...
   (各种大小混合)
```

#### 分配策略对比

| 策略 | 优点 | 缺点 | 适用场景 |
|------|------|------|----------|
| **首次适配<br>(First Fit)** | 快速，简单 | 容易产生小碎片 | 通用场景 |
| **最佳适配<br>(Best Fit)** | 最小化浪费 | 搜索慢，产生微小碎片 | 内存受限 |
| **最坏适配<br>(Worst Fit)** | 剩余块仍然可用 | 快速耗尽大块 | 特殊场景 |
| **下次适配<br>(Next Fit)** | 平衡速度和碎片 | 可能产生更多碎片 | 循环分配 |

#### 演示内存碎片

```c
#include <stdio.h>
#include <stdlib.h>

void demonstrate_fragmentation() {
    printf("演示内存碎片\n\n");
    
    // 分配 10 个 100 字节的块
    void *blocks[10];
    for (int i = 0; i < 10; i++) {
        blocks[i] = malloc(100);
        printf("分配块 %d: %p\n", i, blocks[i]);
    }
    
    // 释放偶数位置的块（创建碎片）
    for (int i = 0; i < 10; i += 2) {
        free(blocks[i]);
        printf("释放块 %d\n", i);
    }
    
    // 现在堆中有 5 个 100 字节的空闲块（不连续）
    // 尝试分配一个 300 字节的块
    void *large = malloc(300);
    if (large) {
        printf("\n成功分配 300 字节: %p\n", large);
        printf("（可能在堆的新位置，因为碎片空间不足）\n");
        free(large);
    }
    
    // 清理
    for (int i = 1; i < 10; i += 2) {
        free(blocks[i]);
    }
}

int main() {
    demonstrate_fragmentation();
    return 0;
}
```

#### 内存对齐

malloc 返回的地址保证满足最大的对齐要求：

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main() {
    // 测试对齐
    for (int i = 0; i < 10; i++) {
        void *p = malloc(1);  // 只请求 1 字节
        printf("malloc(1) = %p, 对齐到: ", p);
        
        uintptr_t addr = (uintptr_t)p;
        if (addr % 16 == 0) printf("16 字节\n");
        else if (addr % 8 == 0) printf("8 字节\n");
        else printf("其他\n");
        
        free(p);
    }
    
    return 0;
}
// 64 位系统通常对齐到 16 字节
// 32 位系统通常对齐到 8 字节
```

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

### 内存泄漏的实际案例分析

#### 案例 1：Web 服务器中的会话泄漏

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Session {
    char *user_id;
    char *session_data;
    struct Session *next;
} Session;

Session *session_list = NULL;

// 错误版本：会话创建后从不释放
Session* create_session_bad(const char *user_id) {
    Session *s = malloc(sizeof(Session));
    s->user_id = strdup(user_id);
    s->session_data = malloc(1024);  // 1KB 会话数据
    s->next = session_list;
    session_list = s;
    
    // 问题：没有提供删除会话的机制
    return s;
}

// 正确版本：提供完整的生命周期管理
Session* create_session_good(const char *user_id) {
    Session *s = malloc(sizeof(Session));
    if (!s) return NULL;
    
    s->user_id = strdup(user_id);
    if (!s->user_id) {
        free(s);
        return NULL;
    }
    
    s->session_data = malloc(1024);
    if (!s->session_data) {
        free(s->user_id);
        free(s);
        return NULL;
    }
    
    s->next = session_list;
    session_list = s;
    return s;
}

void destroy_session(const char *user_id) {
    Session **curr = &session_list;
    while (*curr) {
        if (strcmp((*curr)->user_id, user_id) == 0) {
            Session *to_delete = *curr;
            *curr = (*curr)->next;
            
            free(to_delete->user_id);
            free(to_delete->session_data);
            free(to_delete);
            return;
        }
        curr = &(*curr)->next;
    }
}

void cleanup_all_sessions() {
    while (session_list) {
        Session *temp = session_list;
        session_list = session_list->next;
        
        free(temp->user_id);
        free(temp->session_data);
        free(temp);
    }
}

// 模拟长时间运行的服务器
void simulate_server() {
    printf("模拟 Web 服务器运行...\n");
    
    for (int i = 0; i < 10000; i++) {
        char user_id[32];
        snprintf(user_id, sizeof(user_id), "user%d", i);
        
        // 创建会话
        create_session_good(user_id);
        
        // 模拟：一半的会话应该被删除
        if (i % 2 == 0) {
            destroy_session(user_id);
        }
        
        if (i % 1000 == 0) {
            printf("已处理 %d 个请求\n", i);
        }
    }
    
    cleanup_all_sessions();
    printf("服务器清理完成\n");
}
```

#### 案例 2：文件描述符泄漏

```c
#include <stdio.h>
#include <stdlib.h>

// 错误：可能泄漏文件描述符
char* read_file_bad(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return NULL;
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *buffer = malloc(size + 1);
    if (!buffer) {
        // 错误：忘记关闭文件
        return NULL;  // 文件描述符泄漏！
    }
    
    fread(buffer, 1, size, fp);
    buffer[size] = '\0';
    
    fclose(fp);
    return buffer;
}

// 正确：使用 goto 确保资源释放
char* read_file_good(const char *filename) {
    FILE *fp = NULL;
    char *buffer = NULL;
    long size;
    
    fp = fopen(filename, "r");
    if (!fp) goto cleanup;
    
    if (fseek(fp, 0, SEEK_END) != 0) goto cleanup;
    size = ftell(fp);
    if (size < 0) goto cleanup;
    fseek(fp, 0, SEEK_SET);
    
    buffer = malloc(size + 1);
    if (!buffer) goto cleanup;
    
    if (fread(buffer, 1, size, fp) != (size_t)size) {
        free(buffer);
        buffer = NULL;
        goto cleanup;
    }
    
    buffer[size] = '\0';
    
cleanup:
    if (fp) fclose(fp);
    return buffer;
}

// 更好：使用 RAII 风格封装
typedef struct {
    FILE *fp;
} FileHandle;

FileHandle* file_open(const char *filename) {
    FileHandle *fh = malloc(sizeof(FileHandle));
    if (!fh) return NULL;
    
    fh->fp = fopen(filename, "r");
    if (!fh->fp) {
        free(fh);
        return NULL;
    }
    
    return fh;
}

void file_close(FileHandle **fh) {
    if (fh && *fh) {
        if ((*fh)->fp) {
            fclose((*fh)->fp);
        }
        free(*fh);
        *fh = NULL;
    }
}

void demonstrate_file_handling() {
    FileHandle *fh = file_open("test.txt");
    if (fh) {
        // 使用文件...
        file_close(&fh);
        // fh 现在是 NULL，安全
    }
}
```

#### 案例 3：循环引用导致的泄漏

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
    struct Node *prev;  // 双向链表
} Node;

typedef struct List {
    Node *head;
    Node *tail;
    int count;
} List;

List* list_create() {
    List *list = malloc(sizeof(List));
    if (list) {
        list->head = NULL;
        list->tail = NULL;
        list->count = 0;
    }
    return list;
}

void list_append(List *list, int data) {
    Node *node = malloc(sizeof(Node));
    if (!node) return;
    
    node->data = data;
    node->next = NULL;
    node->prev = list->tail;
    
    if (list->tail) {
        list->tail->next = node;
    } else {
        list->head = node;
    }
    
    list->tail = node;
    list->count++;
}

// 错误：只释放列表结构，不释放节点
void list_destroy_bad(List *list) {
    free(list);  // 泄漏：所有节点都没有释放
}

// 正确：递归释放所有节点
void list_destroy_good(List *list) {
    if (!list) return;
    
    Node *current = list->head;
    while (current) {
        Node *next = current->next;
        free(current);
        current = next;
    }
    
    free(list);
}

// 展示泄漏的影响
void demonstrate_list_leak() {
    printf("创建 1000 个列表，每个 1000 个节点...\n");
    
    for (int i = 0; i < 1000; i++) {
        List *list = list_create();
        
        for (int j = 0; j < 1000; j++) {
            list_append(list, j);
        }
        
        // 使用错误的销毁函数
        // list_destroy_bad(list);  // 泄漏约 16MB (1000*1000*16字节)
        
        // 使用正确的销毁函数
        list_destroy_good(list);
        
        if (i % 100 == 0) {
            printf("已处理 %d 个列表\n", i);
        }
    }
    
    printf("完成\n");
}
```

#### 案例 4：多线程环境中的泄漏

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int id;
    char *data;
} ThreadData;

// 错误：线程局部数据从不释放
void* worker_thread_bad(void *arg) {
    ThreadData *data = (ThreadData*)arg;
    
    printf("Thread %d 开始工作\n", data->id);
    sleep(1);
    printf("Thread %d 完成\n", data->id);
    
    // 错误：忘记释放 data->data 和 data
    return NULL;  // 泄漏！
}

// 正确：释放所有分配的内存
void* worker_thread_good(void *arg) {
    ThreadData *data = (ThreadData*)arg;
    
    printf("Thread %d 开始工作\n", data->id);
    sleep(1);
    printf("Thread %d 完成\n", data->id);
    
    // 清理
    free(data->data);
    free(data);
    
    return NULL;
}

void spawn_threads(int num_threads) {
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    
    for (int i = 0; i < num_threads; i++) {
        ThreadData *data = malloc(sizeof(ThreadData));
        data->id = i;
        data->data = malloc(1024);  // 1KB 数据
        
        pthread_create(&threads[i], NULL, worker_thread_good, data);
    }
    
    // 等待所有线程完成
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    free(threads);
}
```

### 高级内存泄漏检测技术

#### 1. 使用 Valgrind 的高级功能

```bash
# 详细的泄漏报告，包括调用栈
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
         ./program

# 生成可视化的调用图
valgrind --tool=callgrind ./program
callgrind_annotate callgrind.out.<pid>

# 或使用 kcachegrind 图形化查看
kcachegrind callgrind.out.<pid>
```

Valgrind 输出解读：
```
==12345== LEAK SUMMARY:
==12345==    definitely lost: 400 bytes in 1 blocks
==12345==    indirectly lost: 800 bytes in 2 blocks
==12345==      possibly lost: 200 bytes in 1 blocks
==12345==    still reachable: 1,000 bytes in 5 blocks
==12345==         suppressed: 0 bytes in 0 blocks

definitely lost: 确定的泄漏，必须修复
indirectly lost: 间接泄漏（通过已泄漏的指针引用）
possibly lost: 可能的泄漏（内部指针）
still reachable: 程序结束时仍可达，通常不是问题
```

#### 2. 使用 AddressSanitizer 的详细选项

```bash
# 编译时启用 ASan
gcc -fsanitize=address \
    -fsanitize=leak \
    -fno-omit-frame-pointer \
    -g -O1 \
    -o program program.c

# 运行时设置选项
ASAN_OPTIONS=detect_leaks=1:leak_check_at_exit=1 ./program

# 其他有用的选项
ASAN_OPTIONS=\
detect_leaks=1:\
leak_check_at_exit=1:\
log_path=asan.log:\
verbosity=1:\
halt_on_error=0:\
print_summary=1 \
./program
```

#### 3. 实现自己的内存跟踪器

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MEMORY_TRACK

typedef struct MemRecord {
    void *ptr;
    size_t size;
    const char *file;
    int line;
    struct MemRecord *next;
} MemRecord;

static MemRecord *mem_records = NULL;
static size_t total_allocated = 0;
static size_t total_freed = 0;
static int allocation_count = 0;
static int free_count = 0;

void* track_malloc(size_t size, const char *file, int line) {
    void *ptr = malloc(size);
    if (!ptr) return NULL;
    
    MemRecord *record = malloc(sizeof(MemRecord));
    if (!record) {
        free(ptr);
        return NULL;
    }
    
    record->ptr = ptr;
    record->size = size;
    record->file = file;
    record->line = line;
    record->next = mem_records;
    mem_records = record;
    
    total_allocated += size;
    allocation_count++;
    
    printf("[ALLOC] %p: %zu bytes at %s:%d (total: %zu bytes, count: %d)\n",
           ptr, size, file, line, total_allocated, allocation_count);
    
    return ptr;
}

void track_free(void *ptr, const char *file, int line) {
    if (!ptr) return;
    
    MemRecord **curr = &mem_records;
    while (*curr) {
        if ((*curr)->ptr == ptr) {
            MemRecord *to_delete = *curr;
            *curr = (*curr)->next;
            
            total_freed += to_delete->size;
            free_count++;
            
            printf("[FREE] %p: %zu bytes at %s:%d (freed: %zu bytes, count: %d)\n",
                   ptr, to_delete->size, file, line, total_freed, free_count);
            
            free(to_delete);
            free(ptr);
            return;
        }
        curr = &(*curr)->next;
    }
    
    printf("[ERROR] 尝试释放未跟踪的指针 %p at %s:%d\n", ptr, file, line);
    free(ptr);  // 仍然释放，避免崩溃
}

void print_memory_leaks() {
    printf("\n========== 内存泄漏报告 ==========\n");
    printf("总分配: %zu bytes (%d 次)\n", total_allocated, allocation_count);
    printf("总释放: %zu bytes (%d 次)\n", total_freed, free_count);
    printf("泄漏: %zu bytes (%d 次)\n", 
           total_allocated - total_freed, 
           allocation_count - free_count);
    
    if (mem_records) {
        printf("\n未释放的内存块:\n");
        MemRecord *curr = mem_records;
        while (curr) {
            printf("  %p: %zu bytes at %s:%d\n",
                   curr->ptr, curr->size, curr->file, curr->line);
            curr = curr->next;
        }
    } else {
        printf("\n没有检测到内存泄漏！✓\n");
    }
    printf("===================================\n");
}

#define malloc(size) track_malloc(size, __FILE__, __LINE__)
#define free(ptr) track_free(ptr, __FILE__, __LINE__)

#endif // MEMORY_TRACK

// 测试代码
int main() {
    int *a = malloc(100);
    int *b = malloc(200);
    char *c = malloc(300);
    
    free(a);
    // 忘记释放 b
    free(c);
    
#ifdef MEMORY_TRACK
    print_memory_leaks();
#endif
    
    return 0;
}
```

编译和运行：
```bash
gcc -DMEMORY_TRACK -o program program.c
./program
```

输出：
```
[ALLOC] 0x5555557592a0: 100 bytes at program.c:120 (total: 100 bytes, count: 1)
[ALLOC] 0x555555759310: 200 bytes at program.c:121 (total: 300 bytes, count: 2)
[ALLOC] 0x5555557593e0: 300 bytes at program.c:122 (total: 600 bytes, count: 3)
[FREE] 0x5555557592a0: 100 bytes at program.c:124 (freed: 100 bytes, count: 1)
[FREE] 0x5555557593e0: 300 bytes at program.c:126 (freed: 400 bytes, count: 2)

========== 内存泄漏报告 ==========
总分配: 600 bytes (3 次)
总释放: 400 bytes (2 次)
泄漏: 200 bytes (1 次)

未释放的内存块:
  0x555555759310: 200 bytes at program.c:121
===================================
```

### 防止内存泄漏的最佳实践

#### 1. 使用智能指针模式（C 中模拟）

```c
#include <stdio.h>
#include <stdlib.h>

// 定义清理函数类型
typedef void (*cleanup_func_t)(void*);

// 智能指针结构
typedef struct {
    void *ptr;
    cleanup_func_t cleanup;
} smart_ptr_t;

// 创建智能指针
smart_ptr_t smart_ptr_create(void *ptr, cleanup_func_t cleanup) {
    smart_ptr_t sp = {ptr, cleanup};
    return sp;
}

// 释放智能指针
void smart_ptr_destroy(smart_ptr_t *sp) {
    if (sp && sp->ptr) {
        if (sp->cleanup) {
            sp->cleanup(sp->ptr);
        }
        sp->ptr = NULL;
    }
}

// 使用宏简化语法
#define AUTO_FREE __attribute__((cleanup(auto_free_cleanup)))

void auto_free_cleanup(void *p) {
    void **ptr = (void**)p;
    if (*ptr) {
        free(*ptr);
        *ptr = NULL;
    }
}

void example_with_auto_free() {
    AUTO_FREE char *str = malloc(100);
    AUTO_FREE int *numbers = malloc(10 * sizeof(int));
    
    // 使用 str 和 numbers...
    
    // 函数结束时自动释放，无需手动 free
}

// GCC 特有的 cleanup 属性示例
void demonstrate_cleanup_attribute() {
    __attribute__((cleanup(auto_free_cleanup))) 
    char *buffer = malloc(1024);
    
    if (buffer) {
        strcpy(buffer, "Hello, World!");
        printf("%s\n", buffer);
    }
    
    // buffer 自动释放
}
```

#### 2. 资源获取即初始化（RAII）模式

```c
#include <stdio.h>
#include <stdlib.h>

// 文件资源封装
typedef struct {
    FILE *fp;
    char *buffer;
} FileResource;

FileResource* file_resource_create(const char *filename) {
    FileResource *res = malloc(sizeof(FileResource));
    if (!res) return NULL;
    
    res->fp = fopen(filename, "r");
    if (!res->fp) {
        free(res);
        return NULL;
    }
    
    res->buffer = malloc(4096);
    if (!res->buffer) {
        fclose(res->fp);
        free(res);
        return NULL;
    }
    
    return res;
}

void file_resource_destroy(FileResource **res) {
    if (res && *res) {
        if ((*res)->buffer) free((*res)->buffer);
        if ((*res)->fp) fclose((*res)->fp);
        free(*res);
        *res = NULL;
    }
}

void process_file_safe(const char *filename) {
    FileResource *res = file_resource_create(filename);
    if (!res) return;
    
    // 处理文件...
    
    file_resource_destroy(&res);
    // res 现在是 NULL
}
```

#### 3. 内存池技术

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct MemoryPool {
    void *memory;
    size_t size;
    size_t used;
    size_t peak_usage;
} MemoryPool;

MemoryPool* pool_create(size_t size) {
    MemoryPool *pool = malloc(sizeof(MemoryPool));
    if (!pool) return NULL;
    
    pool->memory = malloc(size);
    if (!pool->memory) {
        free(pool);
        return NULL;
    }
    
    pool->size = size;
    pool->used = 0;
    pool->peak_usage = 0;
    
    return pool;
}

void* pool_alloc(MemoryPool *pool, size_t size) {
    // 对齐到 8 字节边界
    size = (size + 7) & ~7;
    
    if (pool->used + size > pool->size) {
        printf("内存池空间不足\n");
        return NULL;
    }
    
    void *ptr = (char*)pool->memory + pool->used;
    pool->used += size;
    
    if (pool->used > pool->peak_usage) {
        pool->peak_usage = pool->used;
    }
    
    return ptr;
}

void pool_reset(MemoryPool *pool) {
    pool->used = 0;
}

void pool_destroy(MemoryPool *pool) {
    if (pool) {
        printf("内存池统计:\n");
        printf("  总大小: %zu bytes\n", pool->size);
        printf("  峰值使用: %zu bytes (%.1f%%)\n", 
               pool->peak_usage, 
               (double)pool->peak_usage / pool->size * 100);
        
        free(pool->memory);
        free(pool);
    }
}

// 使用示例
void use_memory_pool() {
    MemoryPool *pool = pool_create(1024 * 1024);  // 1MB
    
    // 分配多个对象
    int *numbers = pool_alloc(pool, 100 * sizeof(int));
    char *string = pool_alloc(pool, 256);
    
    // 使用对象...
    
    // 重置池（不需要单独释放每个对象）
    pool_reset(pool);
    
    // 可以重新使用
    float *floats = pool_alloc(pool, 50 * sizeof(float));
    
    pool_destroy(pool);
}

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
