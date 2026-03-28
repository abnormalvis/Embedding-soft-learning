# packaged_task

## 目录
1. [packaged_task 是什么](#packaged_task-是什么)
2. [基础示例](#基础示例)
3. [与 thread 配合](#与-thread-配合)
4. [与线程池关系](#与线程池关系)
5. [常见坑](#常见坑)

---

## packaged_task 是什么

packaged_task 把“可调用对象”封装成可异步执行任务，并提供 future 获取结果。

可理解为：

- 任务包装器（callable wrapper）
- 自动关联 future

---

## 基础示例

```cpp
#include <future>
#include <iostream>

int main() {
    std::packaged_task<int(int, int)> task([](int a, int b) { return a + b; });
    auto f = task.get_future();

    task(3, 4);
    std::cout << f.get() << "\n";
}
```

---

## 与 thread 配合

```cpp
#include <future>
#include <iostream>
#include <thread>

int mul(int a, int b) { return a * b; }

int main() {
    std::packaged_task<int(int, int)> task(mul);
    auto f = task.get_future();

    std::thread t(std::move(task), 6, 7);
    std::cout << f.get() << "\n";
    t.join();
}
```

---

## 与线程池关系

线程池常用 packaged_task 封装提交任务，然后把 future 返回给调用方，实现“异步提交 + 同步取值”。

---

## 常见坑

1. packaged_task 只能执行一次。
2. 忘记 move 到 thread/容器，触发编译错误。
3. 没有 get_future 就执行，结果无法回收。
4. 任务内部异常未处理，get 时抛出。
