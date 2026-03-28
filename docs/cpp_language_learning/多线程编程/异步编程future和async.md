# 异步编程：future 和 async

## 目录
1. [核心概念](#核心概念)
2. [std::future](#stdfuture)
3. [std::async](#stdasync)
4. [launch 策略](#launch-策略)
5. [异常传播](#异常传播)
6. [常见坑](#常见坑)

---

## 核心概念

- future：承载“未来结果”的对象。
- async：启动异步任务并返回 future。

它们适合“有返回值的异步任务”。

---

## std::future

future 主要操作：

- get()：阻塞直到结果可用，并取出结果。
- wait()/wait_for()：等待结果但暂不取值。

```cpp
#include <future>
#include <iostream>

int work() { return 42; }

int main() {
    std::future<int> f = std::async(std::launch::async, work);
    std::cout << f.get() << "\n";
}
```

---

## std::async

```cpp
auto f = std::async(std::launch::async, [] { return heavy_task(); });
```

优点：代码简洁，不需要手动 thread 管理。

---

## launch 策略

- std::launch::async：强制异步线程执行。
- std::launch::deferred：延迟到 get/wait 时在当前线程执行。

建议：业务关键异步任务显式指定 async，避免行为不确定。

---

## 异常传播

异步函数内抛出的异常会在 future.get() 时重新抛出：

```cpp
try {
    auto f = std::async(std::launch::async, []() -> int { throw std::runtime_error("x"); });
    f.get();
} catch (const std::exception& e) {
    // 捕获异步异常
}
```

---

## 常见坑

1. 不调用 get/wait，任务状态无人消费。
2. 误用 deferred 以为开启了新线程。
3. 重复调用 get（future 结果只能取一次）。
4. 异步任务访问了已失效对象。
