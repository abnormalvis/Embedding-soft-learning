# 宏container_of

container_of 用于在 Linux 内核或底层编程中，根据结构体成员的指针反向推导出整个结构体的起始地址。以下是逐行解析：
---
## 宏定义
```
#define container_of(ptr, type, member) ({          \
    const typeof(((type *)0)->member) *__mptr = (ptr); \
    (type *)((char *)__mptr - offsetof(type, member)); })
```

## 参数说明

- ptr：结构体成员 member 的指针（已知地址）。

- type：目标结构体的类型（如 struct my_struct）。

- member：结构体中的成员名称（与 ptr 对应的成员）。

## 代码解析

### 第 1 行：类型安全的指针声明
```
const typeof(((type *)0)->member) *__mptr = (ptr);
```

- (type *)0：  

  将地址 0 强制转换为 type* 类型（不实际访问内存，仅用于类型推导）。
- ((type *)0)->member：  

  获取结构体 type 中成员 member 的类型（例如 int、char 等）。
- typeof(...)：  

  GCC 扩展关键字，获取成员的类型（如 typeof(int) 得到 int）。
- const typeof(...) *__mptr = (ptr)：  

  声明一个常量指针 __mptr，类型为成员类型的指针，并赋值为 ptr。  
  作用：  
  1. 类型检查：确保 ptr 的类型与成员类型匹配（编译器会警告类型错误）。  
  2. 创建临时指针变量，便于后续计算。

### 第 2 行：计算结构体地址
```
(type *)((char *)__mptr - offsetof(type, member));
```
- offsetof(type, member)：  

  标准宏（定义在 <stddef.h>），计算成员 member 在结构体 type 中的偏移量（字节单位）。  
  等价于：(size_t)(&(((type*)0)->member))。
- (char *)__mptr：  

  将成员指针转换为 char*（保证指针算术按字节进行）。
- (char *)__mptr - offsetof(...)：  

  从成员地址回退偏移量，得到结构体起始地址。
- (type *)...：  

  将结果转换为目标结构体指针类型。

## 核心原理

结构体地址 = 成员地址 - 成员在结构体中的偏移量
---
- 成员偏移量：编译时确定（通过 offsetof 计算）。

- 示例：
  struct Point {
      int x;  // 偏移量 0
      int y;  // 偏移量 4
  };
  struct Point p = {10, 20};
  int *y_ptr = &p.y;  // y_ptr = p 的地址 + 4
  
  // 通过 container_of(y_ptr, struct Point, y) 计算 p 的地址：
  //   __mptr = y_ptr (指向 20)
  //   偏移量 = offsetof(struct Point, y) = 4
  //   结构体地址 = (char*)y_ptr - 4 = p 的原始地址
  

## 关键点

1. 类型安全：  
   使用 typeof 确保 ptr 类型与成员类型一致。
2. 避免硬编码：  
   通过 offsetof 动态计算偏移量，适应不同平台/编译器。
3. GCC 扩展：  
   依赖 typeof 和语句表达式 ({ ... })（非标准 C，但广泛用于内核/GCC）。
4. 用途：  
   常用于链表、驱动等场景，通过成员指针反向访问整个结构体。

## 等价简化版
```
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))
```
**简化版（原始版本增加了类型检查和临时变量，更安全）**

## 注意事项

- ptr 不能为 NULL：否则地址计算错误。

- 成员必须属于结构体：否则行为未定义。

- 严格别名规则：需确保指针操作合法（通常无问题）。

** 此宏是底层开发的经典技巧，体现了指针运算和结构体内存布局的核心思想。**