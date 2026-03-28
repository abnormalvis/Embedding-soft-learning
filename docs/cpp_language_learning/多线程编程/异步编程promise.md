# 异步编程：promise

## 目录
1. [promise 是什么](#promise-是什么)
2. [基本用法](#基本用法)
3. [传递异常](#传递异常)
4. [适用场景](#适用场景)
5. [常见坑](#常见坑)

---

## promise 是什么

promise 是“结果生产端”，future 是“结果消费端”。

- 生产端：set_value / set_exception
- 消费端：future.get

它适用于“线程 A 计算，线程 B 获取结果”的手动协作模型。

---

## 基本用法

```cpp
#include <future>
#include <iostream>
#include <thread>

void producer(std::promise<int> p) {
    p.set_value(123);
}

int main() {
    std::promise<int> p;
    std::future<int> f = p.get_future();

    std::thread t(producer, std::move(p));
    std::cout << f.get() << "\n";
    t.join();
}
```

---

## 传递异常

```cpp
void producer(std::promise<int> p) {
    try {
        throw std::runtime_error("fail");
    } catch (...) {
        p.set_exception(std::current_exception());
    }
}
```

消费端在 get() 时会收到异常。

---

## 适用场景

- 需要手动控制何时发布结果。
- 一个线程执行复杂流程后回传状态。
- 与自定义线程池/事件系统联动。

---

## 常见坑

1. promise 未 set_value 或 set_exception，future.get 永久阻塞。
2. promise 被销毁过早，future 报 broken_promise。
3. 同一个 promise 重复 set_value。
4. 忘记 std::move 传递 promise 给线程。
