# 指针深入解析

## 地址与取址操作
- 每个变量在内存中有唯一地址。
- `&` 运算符获取变量地址。
- `*` 运算符解引用指针，访问指向的值。

```c
int value = 42;
int *ptr = &value;
printf("address=%p content=%d\n", (void *)ptr, *ptr);
*ptr = 100;
printf("value is now %d\n", value);
```

## 指针变量定义
- 语法：`类型 *指针名;`
- 指针本身也是变量，占用内存（32位系统4字节，64位8字节）。

```c
int *p1;           // 未初始化，危险
int *p2 = NULL;    // 空指针
int x = 5;
int *p3 = &x;      // 指向 x
```

## 指针偏移应用
- 指针算术：`ptr + n` 移动 `n * sizeof(*ptr)` 字节。
- 常用于数组遍历、缓冲区处理。

```c
int nums[5] = {10, 20, 30, 40, 50};
int *p = nums;
for (int i = 0; i < 5; ++i) {
    printf("%d ", *(p + i));
}
```

## 指针与数组
- 数组名在表达式中退化为指向首元素的指针。
- `arr[i]` 等价于 `*(arr + i)`。
- 指针可以像数组一样使用下标。

```c
int data[] = {1, 2, 3};
int *p = data;
printf("p[0]=%d p[1]=%d\n", p[0], p[1]);
```

## 指针与结构体
- 使用 `->` 访问结构体成员：`ptr->member` 等价于 `(*ptr).member`。

```c
struct Point {
    int x, y;
};

struct Point pt = {10, 20};
struct Point *ptr = &pt;
ptr->x = 30;
printf("(%d, %d)\n", ptr->x, ptr->y);
```

## 指针与函数
- **函数参数传递**：通过指针修改外部变量。
- **返回指针**：返回动态分配的内存或静态数据。

```c
void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int *create_int(int value) {
    int *p = malloc(sizeof(int));
    if (p) {
        *p = value;
    }
    return p;
}
```

## 函数指针与回调
- 函数指针存储函数地址，实现回调、策略模式。

```c
typedef void (*callback_fn)(int);

void process(int value, callback_fn cb) {
    cb(value * 2);
}

void print_int(int x) {
    printf("value=%d\n", x);
}

process(5, print_int);
```

## 指针与常量 const
- `const int *p`：指针指向的内容不可修改。
- `int * const p`：指针本身不可修改。
- `const int * const p`：都不可修改。

```c
int x = 10, y = 20;
const int *p1 = &x;
// *p1 = 30;  // 错误
p1 = &y;      // 允许

int * const p2 = &x;
*p2 = 30;     // 允许
// p2 = &y;   // 错误
```

## 多级指针
- `int **pp` 指向 `int *` 的指针。
- 常用于动态二维数组、函数需修改指针本身。

```c
int value = 42;
int *p = &value;
int **pp = &p;
printf("**pp=%d\n", **pp);
```

## 野指针与悬空指针
- **野指针**：未初始化或指向已释放的内存。
- **悬空指针**：指向的对象已销毁。
- 防范：初始化为 `NULL`、释放后置 `NULL`、检查有效性。

```c
int *p = NULL;
if (p) {
    *p = 10;  // 安全检查
}
```
