# 函数适配器与可调用对象封装

## 目录
1. [核心概念](#核心概念)
2. [std::bind](#stdbind)
3. [std::function](#stdfunction)
4. [std::mem_fn 与成员函数调用](#stdmem_fn-与成员函数调用)
5. [实践建议](#实践建议)
6. [常见坑](#常见坑)

---

## 核心概念

函数适配器用于“改造可调用对象的调用方式”，包括：

- 固定部分参数
- 延迟绑定调用目标
- 把成员函数转成可统一调用形式

C++11 起，lambda 覆盖了很多传统适配器场景，但 `std::bind`、`std::function` 仍然常见。

---

## std::bind

`std::bind` 可以预绑定参数，使用占位符 `_1`、`_2` 表示未来传入参数。

```cpp
#include <functional>
#include <iostream>

int add3(int a, int b, int c) {
    return a + b + c;
}

int main() {
    auto f = std::bind(add3, 10, std::placeholders::_1, std::placeholders::_2);
    std::cout << f(20, 30) << "\n"; // 60
}
```

---

## std::function

`std::function<R(Args...)>` 是类型擦除的可调用包装器，可统一存放函数、lambda、仿函数、bind 结果。

```cpp
#include <functional>
#include <iostream>

int main() {
    std::function<int(int, int)> op;

    op = [](int a, int b) { return a + b; };
    std::cout << op(2, 3) << "\n";

    op = [](int a, int b) { return a * b; };
    std::cout << op(2, 3) << "\n";
}
```

---

## std::mem_fn 与成员函数调用

```cpp
#include <functional>
#include <iostream>

struct Printer {
    void print(int x) const {
        std::cout << "value: " << x << "\n";
    }
};

int main() {
    Printer p;
    auto mf = std::mem_fn(&Printer::print);
    mf(p, 42);
}
```

适用于需要把成员函数指针接入通用算法/框架的场景。

---

## 实践建议

- 简单场景优先 lambda，可读性通常更好。
- 需要统一回调签名时用 `std::function`。
- 绑定开销敏感路径谨慎使用 `std::function`。

---

## 常见坑

1. `std::bind` 占位符顺序写错导致参数错位。
2. `std::function` 为空对象直接调用触发异常（`std::bad_function_call`）。
3. 将生命周期短的对象通过引用绑定，后续调用悬垂引用。
