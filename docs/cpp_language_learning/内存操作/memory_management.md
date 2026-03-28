# C++ 内存管理

## 目录
1. [核心原则](#核心原则)
2. [RAII](#raii)
3. [智能指针](#智能指针)
4. [移动语义与资源转移](#移动语义与资源转移)
5. [异常安全](#异常安全)
6. [常见问题](#常见问题)

---

## 核心原则

现代 C++ 内存管理原则：

- 优先自动对象生命周期
- 必须动态分配时优先智能指针
- 避免裸 new/delete 直接出现在业务代码

---

## RAII

RAII：资源在构造时获取，在析构时释放。

```cpp
#include <fstream>

void f() {
    std::ifstream in("data.txt");
    // 自动关闭文件
}
```

RAII 是现代 C++ 资源管理基础，不只用于内存，也用于文件、锁、socket。

---

## 智能指针

### unique_ptr

- 独占所有权
- 可移动，不可复制

### shared_ptr

- 引用计数共享所有权
- 有额外计数开销

### weak_ptr

- 弱引用，不增加引用计数
- 用于打破 shared_ptr 循环引用

```cpp
auto p = std::make_unique<int>(42);
```

建议：优先 make_unique / make_shared。

---

## 移动语义与资源转移

移动语义让资源“转移所有权”而非复制资源。

```cpp
std::unique_ptr<int> a = std::make_unique<int>(1);
std::unique_ptr<int> b = std::move(a);
```

a 移动后为空，b 接管资源。

---

## 异常安全

目标：异常发生时不泄漏资源，状态保持一致。

实践：

- 使用 RAII 封装资源
- 减少裸指针
- 先构造后提交（commit/rollback 思维）

---

## 常见问题

1. shared_ptr 循环引用造成泄漏。
2. 混用裸指针和智能指针导致双重释放。
3. 把 unique_ptr 按值传参却忘记 std::move。
4. 在析构函数抛异常。
