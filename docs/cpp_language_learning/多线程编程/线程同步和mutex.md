# 线程同步和 mutex

## 目录
1. [为什么需要同步](#为什么需要同步)
2. [数据竞争示例](#数据竞争示例)
3. [mutex 的基本用法](#mutex-的基本用法)
4. [RAII 锁管理](#raii-锁管理)
5. [常见 mutex 类型](#常见-mutex-类型)
6. [死锁与规避](#死锁与规避)
7. [原子与锁如何选择](#原子与锁如何选择)
8. [练习建议](#练习建议)

---

## 为什么需要同步

多个线程共享同一资源时，如果没有同步保护，访问顺序不可控，会导致数据竞争甚至未定义行为。

同步的目标：

- 保证共享数据一致性。
- 建立可预期的执行顺序。
- 在安全前提下尽量提高并发度。

---

## 数据竞争示例

```cpp
#include <iostream>
#include <thread>

int counter = 0;

void work() {
    for (int i = 0; i < 100000; ++i) {
        ++counter; // 数据竞争
    }
}

int main() {
    std::thread t1(work);
    std::thread t2(work);
    t1.join();
    t2.join();

    std::cout << counter << "\n"; // 结果通常不是 200000
    return 0;
}
```

---

## mutex 的基本用法

```cpp
#include <iostream>
#include <mutex>
#include <thread>

int counter = 0;
std::mutex m;

void work() {
    for (int i = 0; i < 100000; ++i) {
        std::lock_guard<std::mutex> lock(m);
        ++counter;
    }
}

int main() {
    std::thread t1(work);
    std::thread t2(work);
    t1.join();
    t2.join();

    std::cout << counter << "\n"; // 200000
    return 0;
}
```

关键点：不要在业务代码中手写 lock 和 unlock 配对，优先用 RAII 锁对象。

---

## RAII 锁管理

### lock_guard

- 简单、轻量。
- 构造即加锁，析构即解锁。
- 不支持手动解锁或延迟加锁。

### unique_lock

- 更灵活：支持延迟加锁、try_lock、手动 unlock。
- 条件变量等待必须使用 unique_lock。

### scoped_lock（C++17）

- 一次锁多个互斥量。
- 内部使用死锁规避策略，适合多锁场景。

---

## 常见 mutex 类型

| 类型 | 特点 | 典型场景 |
| --- | --- | --- |
| mutex | 最常用，不可重入 | 普通临界区 |
| recursive_mutex | 同线程可重复加锁 | 兼容老代码（不推荐滥用） |
| timed_mutex | 支持超时获取 | 需要降级策略 |
| shared_mutex（C++17） | 读共享、写独占 | 读多写少 |

---

## 死锁与规避

### 典型死锁原因

- 多线程拿锁顺序不一致。
- 锁粒度过大导致长时间占用。
- 锁内调用未知阻塞操作。

### 规避建议

1. 统一锁顺序。
2. 缩短临界区，只保护必须共享的代码。
3. 多锁场景优先使用 scoped_lock。
4. 必要时用 try_lock + 回退策略。

---

## 原子与锁如何选择

- 仅对单个标量做简单读写/加减：优先 atomic。
- 涉及多个变量一致性：用 mutex。
- 需要等待条件变化：mutex + condition_variable。

原则：先写对，再优化。不要为“无锁”而无锁。

---

## 练习建议

1. 将计数器示例分别改为 lock_guard、unique_lock、atomic 三个版本并对比性能。
2. 写一个读多写少字典，使用 shared_mutex 优化读路径。
3. 人为制造双锁死锁，再用 scoped_lock 改写并验证修复。
