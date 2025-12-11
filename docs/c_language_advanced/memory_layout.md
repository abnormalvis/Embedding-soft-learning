# 内存空间布局

## 进程内存分区
C 程序运行时的内存分为以下几个区域：

```
高地址
┌──────────────┐
│  命令行参数  │  环境变量
├──────────────┤
│     栈       │  ↓ 向下增长
│              │  局部变量、函数参数、返回地址
├──────────────┤
│      ↕       │  未使用空间
├──────────────┤
│     堆       │  ↑ 向上增长
│              │  malloc/calloc/realloc 分配
├──────────────┤
│  BSS 段      │  未初始化全局/静态变量
├──────────────┤
│  数据段      │  已初始化全局/静态变量
├──────────────┤
│  代码段      │  程序指令（只读）
└──────────────┘
低地址
```

## 代码段（Text Segment）
- 存储程序的机器指令，只读、可共享。
- 常量字符串也通常位于此区域。

```c
const char *msg = "hello";  // "hello" 位于代码段
```

## 数据段（Data Segment）
- 已初始化的全局变量和静态变量。

```c
int global_var = 100;
static int static_var = 200;
```

## BSS 段
- 未初始化或初始化为零的全局/静态变量。
- 不占用可执行文件空间，加载时由系统清零。

```c
int uninitialized_global;
static int static_zero;
```

## 栈（Stack）
- 存储局部变量、函数参数、返回地址。
- 自动分配与释放，先进后出。
- 大小有限（通常 MB 级别），超出会栈溢出。

```c
void func(int param) {
    int local = 10;
    char buffer[1024];
    // param, local, buffer 都在栈上
}
```

### 栈帧（Stack Frame）
- 每次函数调用创建一个栈帧，包含：
  - 局部变量
  - 参数
  - 返回地址
  - 保存的寄存器

## 堆（Heap）
- 动态内存分配区域，程序员通过 `malloc`/`calloc`/`realloc` 申请。
- 生命周期由程序控制，需手动 `free` 释放。
- 分配失败时返回 `NULL`。

```c
int *p = malloc(sizeof(int) * 100);
if (!p) {
    perror("malloc failed");
}
free(p);
```

## 内存泄漏
- 分配的堆内存未释放，导致可用内存逐渐减少。
- 使用 `valgrind` 或 AddressSanitizer 检测。

## 栈溢出
- 递归过深或局部数组过大导致栈空间耗尽。
- 解决：减少递归深度、使用堆分配大数组。

```c
// 危险：可能栈溢出
void recursive(int n) {
    char buffer[10000];
    if (n > 0) {
        recursive(n - 1);
    }
}
```

## 查看内存布局
```bash
# Linux 使用 size 命令
size ./program

# 运行时查看
cat /proc/<pid>/maps
```

## 最佳实践
- 局部变量优先使用栈，生命周期明确。
- 大数据或动态大小使用堆。
- 全局变量尽量少用，避免命名冲突和意外修改。
- 静态局部变量可在函数间保持状态，但要注意线程安全。
