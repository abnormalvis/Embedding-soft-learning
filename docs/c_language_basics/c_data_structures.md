# C数据结构

## struct
- 结构体把不同类型的字段组合在一起，适合描述实体数据。
- 成员在内存中按声明顺序排列，编译器会插入填充字节以满足对齐需求。
- 为减少内存空洞，可将占用字节相近或对齐需求相同的字段放在一起。

```c
#include <stddef.h>

typedef struct {
    char status;      // 1 byte
    int id;           // 4 bytes, 需要 4 字节对齐
    double weight;    // 8 bytes
} Node;

_Static_assert(sizeof(Node) == 16, "Unexpected padding");
```

> 使用 `_Static_assert` 可以在编译阶段验证结构体大小是否符合预期。

### 灵活数组成员
- C99 支持在结构体最后定义一个未指定长度的数组：`char data[];`
- 常用于一次性分配头部和数据区，减少多次 `malloc`。

```c
typedef struct {
    size_t len;
    char data[];
} Blob;

Blob *make_blob(const char *src) {
    size_t n = strlen(src);
    Blob *blob = malloc(sizeof(Blob) + n + 1);
    if (!blob) {
        return NULL;
    }
    blob->len = n;
    memcpy(blob->data, src, n + 1);
    return blob;
}
```

## union
- 共用体的所有成员共享同一块内存，在不同语义间复用存储空间。
- 使用时需要维护当前有效的成员类型，通常配合 `enum` 或标志位。

```c
typedef enum {
    VALUE_INT,
    VALUE_DOUBLE,
    VALUE_STRING
} ValueTag;

typedef struct {
    ValueTag tag;
    union {
        int i;
        double d;
        const char *s;
    } payload;
} Variant;
```

## enum
- 枚举为一组离散常量提供符号名称，提高可读性和可维护性。
- 默认从 `0` 开始递增，也可显式指定数值。
- 可利用枚举值与数组索引映射，实现紧凑的跳转表或元数据表。

```c
typedef enum {
    TOKEN_ADD = '+',
    TOKEN_SUB = '-',
    TOKEN_MUL = '*',
    TOKEN_DIV = '/',
    TOKEN_END = '\0'
} TokenType;
```

## 位域 bit-field
- 位域允许在结构体中定义占用指定位数的成员，常用于协议头或硬件寄存器描述。
- 声明形式：`unsigned flag : 1;`。
- 受编译器和目标平台影响较大，布局和对齐不完全可移植，跨平台时应谨慎使用。

```c
typedef struct {
    unsigned version : 3;
    unsigned type    : 5;
    unsigned length  : 10;
    unsigned flag    : 1;
} PacketHeader;
```

## 内存布局与对齐
- `sizeof` 返回结构体总大小，必定是最大对齐需求的整数倍。
- `offsetof(Type, member)` 宏可以计算成员偏移，帮助验证布局。
- 不要假设结构体没有填充；在序列化或网络传输前应显式拷贝到字节数组。

## 常见实践
- 使用 `typedef` 为结构体创建别名，简化接口定义。
- 将相关操作封装成函数，例如初始化、拷贝、销毁，保持对象一致性。
- 如果结构体需要跨模块暴露，可在头文件只提供前向声明(`typedef struct Foo Foo;`)，在源文件中定义，实现信息隐藏。
