# STL 算法

## 目录
1. [算法库概览](#算法库概览)
2. [非修改序列算法](#非修改序列算法)
3. [修改序列算法](#修改序列算法)
4. [排序与分区算法](#排序与分区算法)
5. [数值算法](#数值算法)
6. [C++20 ranges 概览](#c20-ranges-概览)
7. [常见坑](#常见坑)

---

## 算法库概览

STL 算法主要在 <algorithm>、<numeric> 中，强调“算法与容器解耦”：算法操作迭代器区间，不绑定具体容器。

---

## 非修改序列算法

常用：find / find_if / count_if / all_of / any_of

```cpp
#include <algorithm>
#include <vector>

std::vector<int> v{1, 2, 3, 4, 5};
auto it = std::find_if(v.begin(), v.end(), [](int x) { return x > 3; });
```

---

## 修改序列算法

常用：copy / transform / replace_if / remove_if

```cpp
std::vector<int> v{1,2,3,4,5};
std::vector<int> out(v.size());
std::transform(v.begin(), v.end(), out.begin(), [](int x){ return x * x; });
```

注意 remove/remove_if 只是“逻辑移除”，通常要配合 erase：

```cpp
v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
```

---

## 排序与分区算法

常用：sort / stable_sort / partial_sort / nth_element / partition

```cpp
std::sort(v.begin(), v.end());
```

- sort：平均 O(n log n)
- stable_sort：稳定排序，通常额外内存更多

---

## 数值算法

在 <numeric>：

- accumulate
- inner_product
- partial_sum

```cpp
#include <numeric>
int sum = std::accumulate(v.begin(), v.end(), 0);
```

---

## C++20 ranges 概览

ranges 让算法调用更直观：

```cpp
#include <algorithm>
#include <ranges>

std::ranges::sort(v);
```

优势：

- 更强类型约束
- 可组合视图（views）
- 代码可读性更高

---

## 常见坑

1. 忽略迭代器失效规则。
2. remove 后忘记 erase。
3. 在 unordered 容器上误用依赖顺序的算法假设。
4. 过早微优化，忽略算法复杂度主导项。
