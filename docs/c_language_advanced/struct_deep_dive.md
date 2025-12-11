# 结构体深度解析

## 定义与初始化
```c
struct Book {
    int id;
    char title[64];
    double price;
};

struct Book book1 = {1, "C Programming", 59.9};
struct Book book2 = {.id = 2, .price = 39.5, .title = "Linux Basics"};
```

## 内存布局与对齐
- 编译器为提高访问效率，会在成员间插入填充字节。
- 结构体总大小是最大对齐需求的整数倍。

```c
struct A {
    char c;    // 1 byte
    int i;     // 4 bytes, 需 4 字节对齐
    char d;    // 1 byte
};
// 实际大小可能是 12 字节而非 6 字节
```

### 优化布局
- 将相同大小或对齐需求的成员放在一起可减少浪费。

```c
struct Optimized {
    int i;
    int j;
    char a;
    char b;
};
```

## 结构体嵌套（内嵌外挂）
- **内嵌**：直接包含另一个结构体作为成员。
- **外挂**：使用指针引用外部结构体，节省空间或实现共享。

```c
typedef struct {
    int x, y;
} Point;

typedef struct {
    Point top_left;      // 内嵌
    Point *center;       // 外挂
    int width, height;
} Rectangle;

Point c = {50, 50};
Rectangle rect = {{0, 0}, &c, 100, 80};
printf("center at (%d, %d)\n", rect.center->x, rect.center->y);
```

## 结构体数组
- 创建多个相同类型对象的集合。

```c
struct Student students[3] = {
    {1, "Alice", 90.5},
    {2, "Bob", 85.0},
    {3, "Carol", 92.3}
};

for (int i = 0; i < 3; ++i) {
    printf("ID=%d Name=%s Score=%.1f\n", students[i].id, students[i].name, students[i].score);
}
```

## 结构体指针
- 使用 `->` 操作符访问成员，等价于 `(*ptr).member`。

```c
struct Student *ptr = &students[0];
ptr->score += 5.0;
printf("updated score=%.1f\n", ptr->score);
```

## 柔性数组成员
- C99 允许结构体最后一个成员为未指定大小的数组。

```c
typedef struct {
    size_t count;
    int data[];
} IntArray;

IntArray *arr = malloc(sizeof(IntArray) + 10 * sizeof(int));
arr->count = 10;
for (size_t i = 0; i < arr->count; ++i) {
    arr->data[i] = (int)i;
}
free(arr);
```

## 位域
- 在结构体中以位为单位分配成员，节省空间。

```c
struct Flags {
    unsigned int is_active : 1;
    unsigned int has_permission : 1;
    unsigned int level : 4;
};
```

## 应用场景
- **数据库记录**：表示一行数据。
- **图形对象**：顶点、矩形、圆等几何形状。
- **链表节点**：包含数据和指向下一节点的指针。
- **配置结构**：程序设置、参数集合。
