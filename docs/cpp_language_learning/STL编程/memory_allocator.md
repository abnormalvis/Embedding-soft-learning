# STL 分配器（Allocator）入门

## 目录
1. [分配器是什么](#分配器是什么)
2. [默认分配器行为](#默认分配器行为)
3. [自定义分配器基本形态](#自定义分配器基本形态)
4. [什么时候值得自定义](#什么时候值得自定义)
5. [常见坑](#常见坑)

---

## 分配器是什么

Allocator 是容器进行内存申请/释放时使用的策略对象。

例如：

```cpp
std::vector<int, std::allocator<int>> v;
```

大多数情况下使用默认分配器即可。

---

## 默认分配器行为

标准容器通过 `allocator_traits` 调用分配器接口，统一处理：

- `allocate` / `deallocate`
- 对象构造与销毁（经 traits 适配）

这使容器实现与具体分配器解耦。

---

## 自定义分配器基本形态

```cpp
#include <cstddef>
#include <memory>

template <typename T>
struct MyAllocator {
    using value_type = T;

    MyAllocator() = default;

    template <typename U>
    MyAllocator(const MyAllocator<U>&) {}

    T* allocate(std::size_t n) {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t) noexcept {
        ::operator delete(p);
    }
};
```

该示例展示最小可用骨架，真实工程通常还要考虑对齐、统计、线程安全与资源回收策略。

---

## 什么时候值得自定义

- 需要对象池/内存池降低频繁分配开销
- 需要跟踪内存来源与统计信息
- 需要特定 NUMA/共享内存策略

如果没有明确性能或部署诉求，不建议过早自定义。

---

## 常见坑

1. 分配器实现不满足容器要求，触发隐蔽 UB。
2. 忽略异常安全，`allocate` 失败路径处理不完整。
3. 跨分配器实例释放内存，导致未定义行为。
4. 以为自定义分配器一定更快，实际可能更慢。
