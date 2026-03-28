# 仿函数（函数对象）

## 目录
1. [概念与定位](#概念与定位)
2. [为什么不只用普通函数](#为什么不只用普通函数)
3. [基础示例](#基础示例)
4. [在 STL 算法中的常见用法](#在-stl-算法中的常见用法)
5. [与 Lambda 的关系](#与-lambda-的关系)
6. [常见坑](#常见坑)

---

## 概念与定位

仿函数（functor）是重载了 `operator()` 的类型对象。

```cpp
struct Add {
    int operator()(int a, int b) const {
        return a + b;
    }
};
```

调用方式与函数类似：

```cpp
Add add;
int x = add(2, 3); // 5
```

---

## 为什么不只用普通函数

仿函数相对普通函数最大的优势是“可携带状态”。

```cpp
struct Threshold {
    int limit;

    explicit Threshold(int n) : limit(n) {}

    bool operator()(int x) const {
        return x > limit;
    }
};
```

同一个类型可以创建不同对象，封装不同阈值。

---

## 基础示例

```cpp
#include <iostream>
#include <vector>
#include <algorithm>

struct Counter {
    int calls = 0;

    bool operator()(int x) {
        ++calls;
        return x % 2 == 0;
    }
};

int main() {
    std::vector<int> v{1, 2, 3, 4, 5, 6};
    Counter c;

    int cnt = std::count_if(v.begin(), v.end(), c);
    std::cout << "even count: " << cnt << "\n";
    std::cout << "predicate calls in c: " << c.calls << "\n";
}
```

注意：算法通常按值接收谓词，示例中的 `c.calls` 不一定反映算法内部副本的调用次数。

---

## 在 STL 算法中的常见用法

```cpp
#include <vector>
#include <algorithm>
#include <functional>

int main() {
    std::vector<int> v{3, 1, 5, 2, 4};

    std::sort(v.begin(), v.end(), std::greater<int>{});
}
```

`std::greater<int>{}`、`std::less<int>{}` 都是标准库提供的仿函数对象。

---

## 与 Lambda 的关系

现代 C++ 中，很多场景优先用 lambda，但仿函数仍然有价值：

- 需要复用复杂逻辑并命名语义时
- 需要在类型系统中表达行为时（模板参数）
- 需要可配置对象并长期保存状态时

简短一次性逻辑通常用 lambda 更直观。

---

## 常见坑

1. 谓词含可变状态但被算法按值拷贝，状态观察结果与预期不一致。
2. `operator()` 缺少 `const`，导致无法用于需要只读调用对象的场景。
3. 用仿函数替代 lambda 但没有提升可读性，反而增加样板代码。
