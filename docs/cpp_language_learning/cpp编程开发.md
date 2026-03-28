# C++ 现代编程开发学习总览

## 目录
1. [学习目标](#学习目标)
2. [学习路径](#学习路径)
3. [C++ 版本路线图](#c-版本路线图)
4. [开发环境与编译命令](#开发环境与编译命令)
5. [专题导航](#专题导航)
6. [阶段练习建议](#阶段练习建议)
7. [常见坑速查](#常见坑速查)

---

## 学习目标

这套文档面向重新学习 C++ 的同学，强调三件事：

- 先建立现代 C++ 思维，再记语法细节。
- 每个知识点都能落到最小可运行示例。
- 用工程实践标准学习，而不是停留在课堂演示。

学习完成后，你应当能够：

- 使用 C++17/C++20 编写可维护代码。
- 独立处理内存、模板、STL、并发的常见问题。
- 在 Linux 环境下完成编译、调试、排错的闭环。

---

## 学习路径

建议按以下顺序学习（从上到下）：

1. 语言基础与现代特性：类型推导、引用折叠、移动语义、异常安全。
2. 泛型与 STL：容器、算法、迭代器、可调用对象。
3. 资源管理：RAII、智能指针、对象生命周期。
4. 并发编程：线程创建、互斥锁、条件变量、原子操作。
5. 工程实践：构建系统、调试、性能分析、代码规范。

---

## C++ 版本路线图

| 版本 | 重点特性 | 学习优先级 |
| --- | --- | --- |
| C++11 | auto、lambda、move、thread、smart pointer | 必学 |
| C++14 | 泛型 lambda、返回类型推导增强 | 高 |
| C++17 | structured binding、if constexpr、optional、filesystem | 高 |
| C++20 | concepts、ranges、jthread、semaphore/latch/barrier | 中高 |

建议默认使用 C++20 编译选项进行学习，必要时回退到 C++17 验证兼容性。

---

## 开发环境与编译命令

### Linux 推荐环境

- 编译器：g++ 10+（建议 11 或更高）
- 调试器：gdb
- 构建工具：make 或 CMake

### 常用编译命令

```bash
# 单文件调试构建
g++ -std=c++20 -Wall -Wextra -Wpedantic -O0 -g main.cpp -o app

# 启用线程库
g++ -std=c++20 -Wall -Wextra -O2 main.cpp -pthread -o app

# 启用运行时检查（推荐调试阶段）
g++ -std=c++20 -Wall -Wextra -O1 -g -fsanitize=address,undefined main.cpp -o app
```

### 质量基线

- 调试阶段：O0 + g + sanitizer。
- 提交前：至少保证 Wall/Wextra 下无新增告警。
- 并发代码：优先使用 RAII 锁和条件谓词写法。

---

## 专题导航

### 模板与泛型

- [函数模板](./模板/function_template.md)
- [类模板](./模板/class_template.md)

### STL 编程

- [算法](./STL编程/algorithm.md)
- [仿函数](./STL编程/functors.md)
- [函数适配器](./STL编程/function_adapter.md)
- [内存分配器](./STL编程/memory_allocator.md)
- [有序与无序关联容器](./STL编程/unordered_map_and_ordered_set.md)

### 资源与 I/O

- [内存管理](./内存操作/memory_management.md)
- [文件流操作](./文件流操作/files_stream_operations.md)

### 并发编程

- [什么是多线程](./多线程编程/什么是多线程.md)
- [创建多线程](./多线程编程/创建多线程.md)
- [线程同步和 mutex](./多线程编程/线程同步和mutex.md)
- [条件变量](./多线程编程/条件变量.md)
- [线程间同步方式](./多线程编程/线程间同步方式.md)
- [lock_guard](./多线程编程/lock_quard.md)
- [unique_lock](./多线程编程/unique_lock.md)
- [读写锁](./多线程编程/读写锁.md)

---

## 阶段练习建议

### 第一阶段（语言与 STL）

- 实现一个简化版通讯录，要求使用 vector、algorithm、lambda。
- 将原始指针版本重构为 unique_ptr 管理资源。

### 第二阶段（并发）

- 写一个多线程计数器，对比无锁、mutex、atomic 三种实现。
- 写一个生产者-消费者模型，使用 condition_variable 协调。

### 第三阶段（工程）

- 用 CMake 拆分多文件项目。
- 增加单元测试和基础性能测试。

---

## 常见坑速查

- 把 joinable 的 thread 对象直接析构会触发 terminate。
- 条件变量没有谓词会出现虚假唤醒问题。
- 手写 lock/unlock 容易在异常路径泄漏锁。
- shared_ptr 循环引用会导致内存泄漏，需用 weak_ptr 断环。
- 容器扩容后迭代器可能失效，禁止继续使用旧迭代器。

---

## 维护约定

- 本文档仅作为总览与导航，不堆叠大量细节。
- 具体知识点在专题文档维护，避免重复定义。
- 新增主题时，先在本页补目录，再补对应专题文档。
