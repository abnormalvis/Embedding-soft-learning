# 有序与无序关联容器

## 目录
1. [容器家族速览](#容器家族速览)
2. [底层结构与复杂度](#底层结构与复杂度)
3. [如何选型](#如何选型)
4. [示例对比](#示例对比)
5. [实践建议](#实践建议)
6. [常见坑](#常见坑)

---

## 容器家族速览

有序关联容器：

- `std::map`
- `std::set`
- `std::multimap`
- `std::multiset`

无序关联容器：

- `std::unordered_map`
- `std::unordered_set`
- `std::unordered_multimap`
- `std::unordered_multiset`

---

## 底层结构与复杂度

- 有序容器通常基于平衡树，查找/插入/删除平均与最坏多为 `O(log n)`。
- 无序容器通常基于哈希表，平均 `O(1)`，最坏可能退化为 `O(n)`。

有序容器保持键有序，可范围查询；无序容器不保证遍历顺序。

---

## 如何选型

选有序容器：

- 需要有序遍历
- 需要区间查询（`lower_bound` / `upper_bound`）
- 需要稳定的顺序语义

选无序容器：

- 主要是点查找/插入，且追求均摊常数时间
- 不关心元素顺序

---

## 示例对比

```cpp
#include <iostream>
#include <map>
#include <unordered_map>

int main() {
    std::map<int, std::string> om;
    om[3] = "three";
    om[1] = "one";
    om[2] = "two";

    std::unordered_map<int, std::string> um;
    um[3] = "three";
    um[1] = "one";
    um[2] = "two";

    std::cout << "map: ";
    for (const auto& [k, v] : om) {
        std::cout << k << ' ';
    }
    std::cout << "\n";

    std::cout << "unordered_map: ";
    for (const auto& [k, v] : um) {
        std::cout << k << ' ';
    }
    std::cout << "\n";
}
```

`map` 输出为有序键序列，`unordered_map` 输出顺序不固定。

---

## 实践建议

- 哈希容器提前 `reserve` 可减少 rehash。
- 自定义 key 类型时，提供稳定且分布良好的哈希函数。
- 业务依赖顺序时不要使用无序容器。

---

## 常见坑

1. 将无序容器遍历顺序当作稳定顺序使用。
2. 自定义哈希质量差，导致性能退化。
3. rehash 后迭代器失效问题被忽略。
4. 键比较函数或哈希/相等谓词逻辑不一致。
