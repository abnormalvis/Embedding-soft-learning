# unique_lock

## 目录
1. [为什么需要 unique_lock](#为什么需要-unique_lock)
2. [与 lock_guard 的差异](#与-lock_guard-的差异)
3. [基础用法](#基础用法)
4. [构造策略参数](#构造策略参数)
5. [超时与尝试加锁](#超时与尝试加锁)
6. [与条件变量配合](#与条件变量配合)
7. [使用建议](#使用建议)

---

## 为什么需要 unique_lock

当你需要比 lock_guard 更细粒度的控制时，使用 unique_lock：

- 延迟加锁
- 中途解锁
- 尝试加锁
- 超时加锁
- 与条件变量协作等待

---

## 与 lock_guard 的差异

| 能力 | lock_guard | unique_lock |
| --- | --- | --- |
| 自动 RAII 解锁 | 是 | 是 |
| 手动 unlock | 否 | 是 |
| 延迟加锁 defer_lock | 否 | 是 |
| try_lock/超时 | 否 | 是 |
| 条件变量 wait | 否 | 是 |

---

## 基础用法

```cpp
#include <iostream>
#include <mutex>
#include <thread>

std::mutex m;
int value = 0;

void work() {
    std::unique_lock<std::mutex> lock(m);
    ++value;

    lock.unlock();
    // 不需要锁的耗时操作

    lock.lock();
    ++value;
}

int main() {
    std::thread t1(work);
    std::thread t2(work);
    t1.join();
    t2.join();
    std::cout << value << "\n";
    return 0;
}
```

---

## 构造策略参数

```cpp
std::mutex m;
std::unique_lock<std::mutex> a(m);                     // 立即加锁
std::unique_lock<std::mutex> b(m, std::defer_lock);   // 延迟加锁
std::unique_lock<std::mutex> c(m, std::try_to_lock);  // 尝试加锁
```

adopt_lock 只在“你已经手动加过锁”时使用：

```cpp
m.lock();
std::unique_lock<std::mutex> d(m, std::adopt_lock);
```

如果没有预先加锁却使用 adopt_lock，会产生未定义行为。

---

## 超时与尝试加锁

对于 timed_mutex 可以进行超时等待：

```cpp
#include <chrono>
#include <mutex>

std::timed_mutex tm;
std::unique_lock<std::timed_mutex> lock(tm, std::defer_lock);
if (lock.try_lock_for(std::chrono::milliseconds(50))) {
    // 获取成功
} else {
    // 超时降级
}
```

---

## 与条件变量配合

条件变量 wait 需要可解锁再重加锁的锁对象，因此必须用 unique_lock。

```cpp
std::unique_lock<std::mutex> lock(m);
cv.wait(lock, [] { return ready; });
```

---

## 使用建议

1. 默认首选 lock_guard，只有“确实需要灵活控制”再上 unique_lock。
2. 不要把 unique_lock 生命周期写得过长，锁域越短越好。
3. 使用 adopt_lock 前先确认锁状态，否则风险极高。
4. 等待条件时总是使用谓词版本 wait。
