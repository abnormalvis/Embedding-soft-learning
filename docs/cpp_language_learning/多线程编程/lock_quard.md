# lock_guard

## 目录
1. [作用与定位](#作用与定位)
2. [基础用法](#基础用法)
3. [为什么比手写 lock/unlock 更安全](#为什么比手写-lockunlock-更安全)
4. [适用场景](#适用场景)
5. [局限性](#局限性)
6. [练习建议](#练习建议)

---

## 作用与定位

lock_guard 是最轻量的 RAII 锁封装：

- 构造时自动加锁
- 析构时自动解锁

它的目标是降低“忘记解锁”和“异常路径泄漏锁”的风险。

头文件：

```cpp
#include <mutex>
```

---

## 基础用法

```cpp
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

int counter = 0;
std::mutex m;

void add() {
    std::lock_guard<std::mutex> lock(m);
    for (int i = 0; i < 10000; ++i) {
        ++counter;
    }
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(add);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << counter << "\n";
    return 0;
}
```

---

## 为什么比手写 lock/unlock 更安全

反例：

```cpp
m.lock();
// 如果这里抛异常，unlock 可能永远不会执行
m.unlock();
```

改成 lock_guard 后，无论正常返回还是异常，析构都会释放锁。

---

## 适用场景

- 临界区短小且作用域固定。
- 不需要中途解锁。
- 不需要 try_lock 或超时锁。

结论：大多数普通互斥场景，先选 lock_guard。

---

## 局限性

1. 不能手动提前解锁。
2. 不能延迟加锁。
3. 不能与条件变量 wait 直接配合（wait 需要 unique_lock）。

一旦需要以上能力，应切换到 unique_lock。

---

## 练习建议

1. 将同一段临界区代码分别用手写 lock/unlock 与 lock_guard 实现，比较异常安全性。
2. 在锁内故意抛异常，观察 lock_guard 是否可靠释放锁。
3. 把临界区拆成两段，验证为什么 lock_guard 不适合“中途解锁”需求。
