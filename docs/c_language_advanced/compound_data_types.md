# 复合数据类型

## 概念引入
- 基本类型（`int`、`float`、`char`）难以描述复杂实体。
- 复合类型通过组合多个字段表示结构化数据。
- C 提供三种主要复合类型：结构体(`struct`)、联合体(`union`)、枚举(`enum`)。

## 结构体 struct
- 将不同类型的数据打包为一个逻辑单元。
- 成员按声明顺序依次分配内存，编译器会插入填充字节满足对齐需求。

```c
struct Point {
    double x;
    double y;
};

struct Point p1 = {1.0, 2.0};
printf("x=%.1f y=%.1f\n", p1.x, p1.y);
```

## 联合体 union
- 所有成员共享同一块内存，只能同时持有一个有效值。
- 常用于节省空间或实现类型双关（type punning）。

```c
union Data {
    int i;
    float f;
    char bytes[4];
};

union Data d;
d.f = 3.14f;
printf("float=%.2f as_int=%d\n", d.f, d.i);
```

## 枚举 enum
- 为一组相关常量定义符号名称，提高可读性。
- 底层为整型，默认从 0 开始递增。

```c
enum Color { RED, GREEN, BLUE };
enum Color c = GREEN;
if (c == GREEN) {
    puts("color is green");
}
```

## 类型别名 typedef
- 为复杂类型定义简洁别名，简化接口。
- 常与结构体、函数指针配合使用。

```c
typedef struct {
    int id;
    char name[32];
} User;

User alice = {1, "Alice"};
```

## 嵌套复合类型
- 结构体可包含其他结构体、联合体或数组。

```c
typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct {
    char name[32];
    Date birthday;
} Person;

Person bob = {"Bob", {15, 8, 1990}};
printf("%s born on %d/%d/%d\n", bob.name, bob.birthday.day, bob.birthday.month, bob.birthday.year);
```

## 应用场景
- **配置数据**：封装程序设置、选项。
- **协议解析**：网络数据包头部、二进制格式。
- **状态机**：使用枚举表示状态，结构体保存上下文。
- **面向对象模拟**：通过结构体 + 函数指针实现简单的封装和多态。
