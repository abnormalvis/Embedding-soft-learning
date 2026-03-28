# 类模板

## 目录
1. [类模板是什么](#类模板是什么)
2. [基础定义与实例化](#基础定义与实例化)
3. [成员函数实例化机制](#成员函数实例化机制)
4. [类外实现与文件组织](#类外实现与文件组织)
5. [模板与继承](#模板与继承)
6. [常见错误](#常见错误)

---

## 类模板是什么

类模板是“生成类的蓝图”，可根据不同类型参数实例化出不同类类型。

---

## 基础定义与实例化

```cpp
template <typename T>
class Box {
public:
    explicit Box(T v) : value(v) {}
    T get() const { return value; }
private:
    T value;
};

Box<int> a(10);
Box<std::string> b("hello");
```

类模板参数通常不能像函数模板那样自动推导（C++17 有部分类模板实参推导，但不要过度依赖）。

---

## 成员函数实例化机制

类模板成员函数一般按需实例化：

- 只有被用到的成员才会触发具体代码生成。
- 这也是模板编译报错常常“在使用点出现”的原因。

---

## 类外实现与文件组织

推荐：模板声明和定义都放在头文件（或 hpp）里。

```cpp
template <typename T>
class Box {
public:
    explicit Box(T v);
    T get() const;
private:
    T value;
};

template <typename T>
Box<T>::Box(T v) : value(v) {}

template <typename T>
T Box<T>::get() const { return value; }
```

---

## 模板与继承

```cpp
template <typename T>
class Base {
public:
    T data;
};

template <typename T>
class Derived : public Base<T> {};
```

继承时要关注依赖名查找规则（如 this->member），这是模板代码常见陷阱。

---

## 常见错误

1. 模板实现放 cpp 导致链接失败。
2. 依赖名未加 this-> 或 typename。
3. 误把模板特化当普通重载处理。
4. 模板层级过深导致可维护性下降。
