# 指针深入解析与工程实践

## 目录
1. [指针基础](#指针基础)
2. [const 与指针](#const-与指针)
3. [多级指针](#多级指针)
4. [工程项目应用](#工程项目应用)

---

## 指针基础

### 地址与取址操作
- 每个变量在内存中有唯一地址
- `&` 运算符获取变量地址
- `*` 运算符解引用指针，访问指向的值

```c
int value = 42;
int *ptr = &value;
printf("address=%p content=%d\n", (void *)ptr, *ptr);
*ptr = 100;
printf("value is now %d\n", value);
```

### 指针的本质
指针是一个变量，它存储的是另一个变量的内存地址。

```c
#include <stdio.h>

int main() {
    int value = 100;
    int *ptr = &value;
    
    printf("value 的地址: %p\n", (void*)&value);
    printf("ptr 存储的地址: %p\n", (void*)ptr);
    printf("ptr 自身的地址: %p\n", (void*)&ptr);
    printf("ptr 指向的值: %d\n", *ptr);
    
    printf("\nsizeof(int) = %zu\n", sizeof(int));
    printf("sizeof(int*) = %zu\n", sizeof(int*));
    printf("sizeof(ptr) = %zu\n", sizeof(ptr));
    
    return 0;
}
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

---

## const 与指针

### const 指针的四种形式

#### 1. 指向常量的指针（Pointer to const）
```c
const int *ptr;        // 方式1
int const *ptr;        // 方式2（等价）
```
**含义**：指针指向的内容不可修改，但指针本身可以改变指向。

```c
#include <stdio.h>

void demo_pointer_to_const() {
    int x = 10, y = 20;
    const int *ptr = &x;
    
    printf("*ptr = %d\n", *ptr);  // 可以读取
    // *ptr = 30;  // 错误！不能通过 ptr 修改 x 的值
    
    ptr = &y;  // 正确：可以改变指针指向
    printf("*ptr = %d\n", *ptr);
    
    // 但可以直接修改 x
    x = 30;  // 正确
    printf("x = %d\n", x);
}
```

**使用场景**：函数参数，表示函数不会修改传入的数据。

```c
// 函数承诺不修改字符串内容
size_t my_strlen(const char *str) {
    size_t len = 0;
    while (*str != '\0') {
        len++;
        str++;  // 可以移动指针
        // *str = 'x';  // 编译错误！
    }
    return len;
}

// 打印数组，不修改数组内容
void print_array(const int *arr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}
```

#### 2. 常量指针（Const pointer）
```c
int * const ptr = &variable;  // 必须初始化
```
**含义**：指针本身不可修改（不能改变指向），但可以修改指向的内容。

```c
void demo_const_pointer() {
    int x = 10, y = 20;
    int * const ptr = &x;  // 必须初始化
    
    *ptr = 30;  // 正确：可以修改指向的内容
    printf("x = %d\n", x);
    
    // ptr = &y;  // 错误！不能改变指针指向
}
```

**使用场景**：固定数组指针、链表节点的固定引用。

```c
// 缓冲区管理：缓冲区地址固定，但内容可以修改
typedef struct {
    char * const buffer;  // 缓冲区地址固定
    size_t size;
    size_t used;
} Buffer;

Buffer* buffer_create(size_t size) {
    Buffer *buf = malloc(sizeof(Buffer));
    if (!buf) return NULL;
    
    // 初始化时设置固定的缓冲区地址
    *(char**)&buf->buffer = malloc(size);  // 绕过 const 初始化
    if (!buf->buffer) {
        free(buf);
        return NULL;
    }
    
    buf->size = size;
    buf->used = 0;
    return buf;
}
```

#### 3. 指向常量的常量指针（Const pointer to const）
```c
const int * const ptr = &variable;
```
**含义**：指针和指向的内容都不可修改。

```c
void demo_const_pointer_to_const() {
    int x = 10, y = 20;
    const int * const ptr = &x;
    
    printf("*ptr = %d\n", *ptr);  // 可以读取
    // *ptr = 30;  // 错误！不能修改指向的内容
    // ptr = &y;   // 错误！不能修改指针指向
}
```

**使用场景**：只读配置、常量表。

```c
// 系统配置：完全只读
typedef struct {
    const char * const name;
    const int * const value;
} Config;

// 错误代码表：完全只读
static const char * const error_messages[] = {
    "Success",
    "Invalid parameter",
    "Out of memory",
    "File not found"
};

const char * const get_error_message(int code) {
    if (code < 0 || code >= 4) {
        return "Unknown error";
    }
    return error_messages[code];
}
```

#### 4. 普通指针
```c
int *ptr;
```
**含义**：指针和指向的内容都可以修改。

### const 关键字的记忆技巧

**从右往左读**：
```c
const int *p;          // p 是指针，指向 int const（指向的 int 是常量）
int const *p;          // p 是指针，指向 const int（等价上面）
int * const p;         // p 是 const 指针，指向 int（指针是常量）
const int * const p;   // p 是 const 指针，指向 int const（都是常量）
```

**口诀**：
- const 在 `*` 左边：指向的数据是常量
- const 在 `*` 右边：指针本身是常量

### const 在工程中的应用

#### 1. API 设计：明确函数意图
```c
// 好的 API 设计
// 函数承诺不修改输入数据
int string_compare(const char *s1, const char *s2);

// 函数可能修改输入数据
void string_upper(char *str);

// 函数返回只读数据
const char* get_version(void);

// 坏的设计：没有 const，意图不明确
int string_compare(char *s1, char *s2);  // 会修改吗？不清楚！
```

#### 2. 防止意外修改
```c
#include <stdio.h>
#include <string.h>

// 查找字符串中的字符（不应修改原字符串）
const char* find_char(const char *str, char c) {
    while (*str != '\0') {
        if (*str == c) {
            return str;
        }
        str++;
    }
    return NULL;
}

// 使用示例
void example_find() {
    const char *text = "Hello, World!";
    const char *pos = find_char(text, 'W');
    
    if (pos != NULL) {
        printf("Found at position: %ld\n", pos - text);
    }
}
```

#### 3. 与宏和内联函数配合
```c
// 定义常量表
static const int MAX_CONNECTIONS = 100;
static const char * const DEFAULT_HOST = "localhost";

// 配置结构
typedef struct {
    const char *hostname;
    const int port;
    int timeout;  // 可修改
} ServerConfig;

const ServerConfig DEFAULT_CONFIG = {
    .hostname = "localhost",
    .port = 8080,
    .timeout = 30
};
```

#### 4. 类型转换与 const
```c
void demo_const_cast() {
    const char *const_str = "Hello";
    
    // 危险！移除 const 限定符
    char *str = (char*)const_str;
    // str[0] = 'h';  // 未定义行为：修改字符串字面量
    
    // 正确做法：复制到可修改的缓冲区
    char buffer[100];
    strcpy(buffer, const_str);
    buffer[0] = 'h';  // 安全
    printf("%s\n", buffer);
}
```

### const 最佳实践

1. **优先使用 const**：除非需要修改，否则默认使用 const
2. **函数参数**：不修改的参数应该声明为 const
3. **返回值**：返回只读数据使用 const
4. **全局常量**：使用 const 而不是 #define
5. **const 正确性**：整个代码库保持一致的 const 使用

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

### 一级指针回顾
```c
int value = 42;
int *ptr = &value;    // ptr 存储 value 的地址
*ptr = 100;           // 通过 ptr 修改 value
```

**内存布局**：
```
value:    [100]     地址: 0x1000
ptr:      [0x1000]  地址: 0x2000
```

### 二级指针（指针的指针）

#### 基本概念
```c
int value = 42;
int *p1 = &value;     // p1 指向 value
int **p2 = &p1;       // p2 指向 p1（p1 是指针，所以 p2 是二级指针）
```

**内存布局**：
```
value:    [42]       地址: 0x1000
p1:       [0x1000]   地址: 0x2000  （存储 value 的地址）
p2:       [0x2000]   地址: 0x3000  （存储 p1 的地址）
```

**访问方式**：
```c
printf("value = %d\n", value);   // 直接访问：42
printf("*p1 = %d\n", *p1);       // 通过一级指针：42
printf("**p2 = %d\n", **p2);     // 通过二级指针：42

// 修改 value
**p2 = 100;        // 通过二级指针修改
printf("value = %d\n", value);   // 100

// 理解解引用
*p2        // 等于 p1（一级指针）
**p2       // 等于 *p1，即 value
```

#### 二级指针的典型应用

##### 1. 修改指针本身（函数参数）
如果函数需要修改指针本身（而不仅是指针指向的内容），需要传递指针的地址（二级指针）。

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 错误示例：不能修改指针
void allocate_memory_wrong(int *ptr, int size) {
    ptr = malloc(size * sizeof(int));  // 只修改了局部变量 ptr
    if (ptr) {
        ptr[0] = 100;
    }
}

// 正确示例：使用二级指针
void allocate_memory_correct(int **ptr, int size) {
    *ptr = malloc(size * sizeof(int));  // 修改了外部的指针
    if (*ptr) {
        (*ptr)[0] = 100;
    }
}

void demo_modify_pointer() {
    int *data = NULL;
    
    // 错误方式
    allocate_memory_wrong(data, 10);
    if (data == NULL) {
        printf("data is still NULL (wrong way)\n");
    }
    
    // 正确方式
    allocate_memory_correct(&data, 10);
    if (data != NULL) {
        printf("data[0] = %d (correct way)\n", data[0]);
        free(data);
    }
}
```

**为什么需要二级指针？**
- C 语言是值传递，函数参数是局部变量的副本
- 传递 `int *ptr` 时，复制的是指针的值（地址），修改这个副本不影响原指针
- 传递 `int **ptr` 时，可以通过 `*ptr` 访问并修改原指针

##### 2. 动态二维数组
```c
#include <stdio.h>
#include <stdlib.h>

// 创建动态二维数组
int** create_2d_array(int rows, int cols) {
    // 分配行指针数组
    int **array = malloc(rows * sizeof(int*));
    if (array == NULL) return NULL;
    
    // 为每一行分配内存
    for (int i = 0; i < rows; i++) {
        array[i] = malloc(cols * sizeof(int));
        if (array[i] == NULL) {
            // 分配失败，释放已分配的内存
            for (int j = 0; j < i; j++) {
                free(array[j]);
            }
            free(array);
            return NULL;
        }
    }
    
    return array;
}

// 释放二维数组
void free_2d_array(int **array, int rows) {
    if (array == NULL) return;
    
    for (int i = 0; i < rows; i++) {
        free(array[i]);
    }
    free(array);
}

// 使用示例
void demo_2d_array() {
    int rows = 3, cols = 4;
    int **matrix = create_2d_array(rows, cols);
    
    if (matrix != NULL) {
        // 初始化
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                matrix[i][j] = i * cols + j;
            }
        }
        
        // 打印
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                printf("%2d ", matrix[i][j]);
            }
            printf("\n");
        }
        
        free_2d_array(matrix, rows);
    }
}
```

**内存布局**：
```
matrix:     [ptr0] [ptr1] [ptr2]
             ↓      ↓      ↓
ptr0 →      [0] [1] [2] [3]
ptr1 →      [4] [5] [6] [7]
ptr2 →      [8] [9] [10][11]
```

##### 3. 字符串数组（命令行参数）
```c
#include <stdio.h>
#include <string.h>

// main 函数的参数就是二级指针
int main(int argc, char **argv) {  // 或 char *argv[]
    printf("Program name: %s\n", argv[0]);
    printf("Arguments:\n");
    
    for (int i = 1; i < argc; i++) {
        printf("  argv[%d] = %s\n", i, argv[i]);
    }
    
    return 0;
}

// 创建字符串数组
char** create_string_array(const char *strings[], int count) {
    char **array = malloc(count * sizeof(char*));
    if (array == NULL) return NULL;
    
    for (int i = 0; i < count; i++) {
        array[i] = strdup(strings[i]);  // 复制字符串
        if (array[i] == NULL) {
            // 清理已分配的内存
            for (int j = 0; j < i; j++) {
                free(array[j]);
            }
            free(array);
            return NULL;
        }
    }
    
    return array;
}

void free_string_array(char **array, int count) {
    if (array == NULL) return;
    
    for (int i = 0; i < count; i++) {
        free(array[i]);
    }
    free(array);
}
```

##### 4. 链表操作（修改链表头）
```c
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

// 错误：不能修改头指针
void insert_front_wrong(Node *head, int value) {
    Node *new_node = malloc(sizeof(Node));
    if (new_node) {
        new_node->data = value;
        new_node->next = head;
        head = new_node;  // 只修改了局部变量
    }
}

// 正确：使用二级指针
void insert_front_correct(Node **head, int value) {
    Node *new_node = malloc(sizeof(Node));
    if (new_node) {
        new_node->data = value;
        new_node->next = *head;
        *head = new_node;  // 修改了外部的头指针
    }
}

// 删除节点（需要修改前一个节点的 next 指针）
void delete_node(Node **head, int value) {
    Node **indirect = head;  // 二级指针，用于遍历
    
    while (*indirect != NULL) {
        Node *current = *indirect;
        if (current->data == value) {
            *indirect = current->next;  // 修改指针
            free(current);
            return;
        }
        indirect = &(current->next);  // 移动到下一个节点的指针
    }
}

void print_list(Node *head) {
    Node *current = head;
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

void demo_linked_list() {
    Node *list = NULL;
    
    insert_front_correct(&list, 30);
    insert_front_correct(&list, 20);
    insert_front_correct(&list, 10);
    
    print_list(list);  // 10 -> 20 -> 30 -> NULL
    
    delete_node(&list, 20);
    print_list(list);  // 10 -> 30 -> NULL
}
```

### 三级指针及更高级

#### 三级指针示例
```c
int value = 42;
int *p1 = &value;      // 一级：指向 int
int **p2 = &p1;        // 二级：指向 int*
int ***p3 = &p2;       // 三级：指向 int**

printf("***p3 = %d\n", ***p3);  // 42
```

#### 实际应用：动态三维数组
```c
int*** create_3d_array(int x, int y, int z) {
    int ***array = malloc(x * sizeof(int**));
    if (!array) return NULL;
    
    for (int i = 0; i < x; i++) {
        array[i] = malloc(y * sizeof(int*));
        if (!array[i]) {
            // 错误处理...
            return NULL;
        }
        for (int j = 0; j < y; j++) {
            array[i][j] = malloc(z * sizeof(int));
            if (!array[i][j]) {
                // 错误处理...
                return NULL;
            }
        }
    }
    
    return array;
}

// 访问：array[i][j][k]
```

### 多级指针的记忆和理解

#### 记忆技巧
```c
int *p;      // p 是指针，指向 int
int **p;     // p 是指针，指向"指向 int 的指针"
int ***p;    // p 是指针，指向"指向'指向 int 的指针'的指针"
```

#### 解引用规则
```c
int value = 42;
int *p1 = &value;
int **p2 = &p1;
int ***p3 = &p2;

// 每个 * 去掉一层
p1       // int* 类型，值是 value 的地址
*p1      // int 类型，值是 42

p2       // int** 类型，值是 p1 的地址
*p2      // int* 类型，值是 value 的地址（等于 p1）
**p2     // int 类型，值是 42

p3       // int*** 类型
*p3      // int** 类型（等于 p2）
**p3     // int* 类型（等于 p1）
***p3    // int 类型，值是 42
```

### 函数指针数组（高级）
```c
#include <stdio.h>

// 函数指针类型
typedef int (*operation_fn)(int, int);

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }
int divide(int a, int b) { return b != 0 ? a / b : 0; }

void demo_function_pointer_array() {
    // 函数指针数组
    operation_fn operations[] = {add, sub, mul, divide};
    const char *names[] = {"add", "sub", "mul", "div"};
    
    int a = 10, b = 5;
    
    for (int i = 0; i < 4; i++) {
        int result = operations[i](a, b);
        printf("%s(%d, %d) = %d\n", names[i], a, b, result);
    }
}

// 更复杂：函数指针数组的指针
void demo_pointer_to_function_array() {
    operation_fn operations[] = {add, sub, mul, divide};
    operation_fn (*ptr_to_array)[4] = &operations;  // 指向数组的指针
    
    // 使用
    int result = (*ptr_to_array)[0](10, 5);  // 调用 add
    printf("Result: %d\n", result);
}
```

---

## 工程项目应用

### 1. 资源管理系统

#### 场景：文件管理器
使用 const 指针确保文件元数据不被意外修改。

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    const char *filename;      // 文件名不应修改
    const char *owner;         // 所有者不应修改
    size_t size;               // 文件大小可以更新
    time_t last_modified;      // 最后修改时间可以更新
    int ref_count;             // 引用计数
} FileMetadata;

// 创建文件元数据（内部可以设置 const 成员）
FileMetadata* file_metadata_create(const char *filename, const char *owner) {
    FileMetadata *meta = malloc(sizeof(FileMetadata));
    if (!meta) return NULL;
    
    // 使用类型转换设置 const 成员（仅在初始化时）
    *(const char**)&meta->filename = strdup(filename);
    *(const char**)&meta->owner = strdup(owner);
    meta->size = 0;
    meta->last_modified = time(NULL);
    meta->ref_count = 1;
    
    return meta;
}

// 读取文件信息（使用 const 保证不修改）
void file_metadata_print(const FileMetadata *meta) {
    if (!meta) return;
    
    printf("File: %s\n", meta->filename);
    printf("Owner: %s\n", meta->owner);
    printf("Size: %zu bytes\n", meta->size);
    printf("Ref Count: %d\n", meta->ref_count);
}

// 更新文件大小（允许修改非 const 成员）
void file_metadata_update_size(FileMetadata *meta, size_t new_size) {
    if (meta) {
        meta->size = new_size;
        meta->last_modified = time(NULL);
    }
}

// 增加引用计数
void file_metadata_ref(FileMetadata *meta) {
    if (meta) {
        meta->ref_count++;
    }
}

// 减少引用计数，计数为 0 时释放
void file_metadata_unref(FileMetadata **meta) {
    if (!meta || !*meta) return;
    
    (*meta)->ref_count--;
    if ((*meta)->ref_count == 0) {
        free((void*)(*meta)->filename);  // 释放时移除 const
        free((void*)(*meta)->owner);
        free(*meta);
        *meta = NULL;
    }
}
```

### 2. 插件系统

#### 场景：可扩展的命令处理器
使用函数指针和二级指针实现插件注册。

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 命令处理函数类型
typedef int (*command_handler)(const char *args);

// 命令结构
typedef struct {
    const char *name;              // 命令名（只读）
    const char *description;       // 描述（只读）
    command_handler handler;       // 处理函数
} Command;

// 命令管理器
typedef struct {
    Command **commands;    // 二级指针：命令数组
    int count;
    int capacity;
} CommandManager;

// 创建命令管理器
CommandManager* command_manager_create(int initial_capacity) {
    CommandManager *mgr = malloc(sizeof(CommandManager));
    if (!mgr) return NULL;
    
    mgr->commands = malloc(initial_capacity * sizeof(Command*));
    if (!mgr->commands) {
        free(mgr);
        return NULL;
    }
    
    mgr->count = 0;
    mgr->capacity = initial_capacity;
    return mgr;
}

// 注册命令（使用二级指针管理动态数组）
int command_manager_register(CommandManager *mgr, const char *name, 
                             const char *desc, command_handler handler) {
    if (!mgr || !name || !handler) return -1;
    
    // 扩容
    if (mgr->count >= mgr->capacity) {
        int new_capacity = mgr->capacity * 2;
        Command **new_commands = realloc(mgr->commands, 
                                        new_capacity * sizeof(Command*));
        if (!new_commands) return -1;
        
        mgr->commands = new_commands;
        mgr->capacity = new_capacity;
    }
    
    // 创建新命令
    Command *cmd = malloc(sizeof(Command));
    if (!cmd) return -1;
    
    *(const char**)&cmd->name = strdup(name);
    *(const char**)&cmd->description = strdup(desc);
    cmd->handler = handler;
    
    mgr->commands[mgr->count++] = cmd;
    return 0;
}

// 执行命令
int command_manager_execute(const CommandManager *mgr, const char *name, 
                            const char *args) {
    if (!mgr || !name) return -1;
    
    for (int i = 0; i < mgr->count; i++) {
        if (strcmp(mgr->commands[i]->name, name) == 0) {
            return mgr->commands[i]->handler(args);
        }
    }
    
    printf("Command not found: %s\n", name);
    return -1;
}

// 列出所有命令
void command_manager_list(const CommandManager *mgr) {
    if (!mgr) return;
    
    printf("Available commands:\n");
    for (int i = 0; i < mgr->count; i++) {
        printf("  %-15s - %s\n", 
               mgr->commands[i]->name, 
               mgr->commands[i]->description);
    }
}

// 示例命令处理函数
int cmd_help(const char *args) {
    printf("Help command executed\n");
    return 0;
}

int cmd_version(const char *args) {
    printf("Version 1.0.0\n");
    return 0;
}

int cmd_echo(const char *args) {
    printf("Echo: %s\n", args ? args : "");
    return 0;
}

// 使用示例
void demo_plugin_system() {
    CommandManager *mgr = command_manager_create(10);
    
    // 注册命令
    command_manager_register(mgr, "help", "Show help information", cmd_help);
    command_manager_register(mgr, "version", "Show version", cmd_version);
    command_manager_register(mgr, "echo", "Echo a message", cmd_echo);
    
    // 列出命令
    command_manager_list(mgr);
    
    // 执行命令
    command_manager_execute(mgr, "help", NULL);
    command_manager_execute(mgr, "version", NULL);
    command_manager_execute(mgr, "echo", "Hello, World!");
}
```

### 3. 日志系统

#### 场景：多级日志处理
使用 const 和指针实现灵活的日志系统。

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

// 日志级别
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

// 日志级别名称（只读）
static const char * const log_level_names[] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR"
};

// 日志处理器函数类型
typedef void (*log_handler)(LogLevel level, const char *message);

// 日志配置
typedef struct {
    LogLevel min_level;                // 最小日志级别
    log_handler handlers[10];          // 处理器数组
    int handler_count;
} LogConfig;

// 全局日志配置
static LogConfig g_log_config = {
    .min_level = LOG_INFO,
    .handler_count = 0
};

// 默认日志处理器：输出到终端
void log_handler_console(LogLevel level, const char *message) {
    time_t now = time(NULL);
    char time_buf[26];
    ctime_r(&now, time_buf);
    time_buf[24] = '\0';  // 移除换行符
    
    printf("[%s] [%s] %s\n", time_buf, log_level_names[level], message);
}

// 日志处理器：输出到文件
void log_handler_file(LogLevel level, const char *message) {
    FILE *fp = fopen("application.log", "a");
    if (fp) {
        fprintf(fp, "[%s] %s\n", log_level_names[level], message);
        fclose(fp);
    }
}

// 添加日志处理器
void log_add_handler(log_handler handler) {
    if (g_log_config.handler_count < 10) {
        g_log_config.handlers[g_log_config.handler_count++] = handler;
    }
}

// 设置最小日志级别
void log_set_level(LogLevel level) {
    g_log_config.min_level = level;
}

// 记录日志（可变参数）
void log_message(LogLevel level, const char *format, ...) {
    if (level < g_log_config.min_level) {
        return;  // 级别太低，忽略
    }
    
    // 格式化消息
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    // 调用所有处理器
    for (int i = 0; i < g_log_config.handler_count; i++) {
        g_log_config.handlers[i](level, buffer);
    }
}

// 便捷宏
#define LOG_DEBUG(...)   log_message(LOG_DEBUG, __VA_ARGS__)
#define LOG_INFO(...)    log_message(LOG_INFO, __VA_ARGS__)
#define LOG_WARNING(...) log_message(LOG_WARNING, __VA_ARGS__)
#define LOG_ERROR(...)   log_message(LOG_ERROR, __VA_ARGS__)

// 使用示例
void demo_log_system() {
    // 配置日志系统
    log_set_level(LOG_DEBUG);
    log_add_handler(log_handler_console);
    log_add_handler(log_handler_file);
    
    // 使用日志
    LOG_DEBUG("Application started");
    LOG_INFO("Processing request from user %s", "alice");
    LOG_WARNING("Memory usage is high: %d%%", 85);
    LOG_ERROR("Failed to connect to database: %s", "connection timeout");
}
```

### 4. 内存池管理器

#### 场景：高性能内存分配
使用二级指针管理内存池链表。

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 内存块
typedef struct MemBlock {
    void *data;
    size_t size;
    int is_free;
    struct MemBlock *next;
} MemBlock;

// 内存池
typedef struct {
    MemBlock *blocks;     // 块链表
    size_t total_size;    // 总大小
    size_t used_size;     // 已使用大小
} MemoryPool;

// 创建内存池
MemoryPool* mempool_create(size_t size) {
    MemoryPool *pool = malloc(sizeof(MemoryPool));
    if (!pool) return NULL;
    
    pool->blocks = NULL;
    pool->total_size = size;
    pool->used_size = 0;
    
    return pool;
}

// 从内存池分配内存（使用二级指针操作链表）
void* mempool_alloc(MemoryPool *pool, size_t size) {
    if (!pool || size == 0) return NULL;
    
    // 查找空闲块
    MemBlock **indirect = &pool->blocks;
    while (*indirect != NULL) {
        if ((*indirect)->is_free && (*indirect)->size >= size) {
            (*indirect)->is_free = 0;
            return (*indirect)->data;
        }
        indirect = &(*indirect)->next;
    }
    
    // 没有合适的空闲块，分配新块
    if (pool->used_size + size > pool->total_size) {
        return NULL;  // 内存池已满
    }
    
    MemBlock *new_block = malloc(sizeof(MemBlock));
    if (!new_block) return NULL;
    
    new_block->data = malloc(size);
    if (!new_block->data) {
        free(new_block);
        return NULL;
    }
    
    new_block->size = size;
    new_block->is_free = 0;
    new_block->next = NULL;
    
    *indirect = new_block;
    pool->used_size += size;
    
    return new_block->data;
}

// 释放内存（标记为空闲，不真正释放）
void mempool_free(MemoryPool *pool, void *ptr) {
    if (!pool || !ptr) return;
    
    MemBlock *current = pool->blocks;
    while (current != NULL) {
        if (current->data == ptr) {
            current->is_free = 1;
            return;
        }
        current = current->next;
    }
}

// 打印内存池状态
void mempool_stats(const MemoryPool *pool) {
    if (!pool) return;
    
    printf("Memory Pool Statistics:\n");
    printf("  Total Size: %zu bytes\n", pool->total_size);
    printf("  Used Size: %zu bytes\n", pool->used_size);
    printf("  Free Size: %zu bytes\n", pool->total_size - pool->used_size);
    
    int block_count = 0;
    int free_count = 0;
    MemBlock *current = pool->blocks;
    while (current != NULL) {
        block_count++;
        if (current->is_free) free_count++;
        current = current->next;
    }
    
    printf("  Total Blocks: %d\n", block_count);
    printf("  Free Blocks: %d\n", free_count);
}

// 销毁内存池
void mempool_destroy(MemoryPool **pool) {
    if (!pool || !*pool) return;
    
    MemBlock *current = (*pool)->blocks;
    while (current != NULL) {
        MemBlock *next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
    
    free(*pool);
    *pool = NULL;
}

// 使用示例
void demo_memory_pool() {
    MemoryPool *pool = mempool_create(1024 * 1024);  // 1MB 内存池
    
    // 分配内存
    int *arr1 = mempool_alloc(pool, 100 * sizeof(int));
    char *str1 = mempool_alloc(pool, 256);
    double *arr2 = mempool_alloc(pool, 50 * sizeof(double));
    
    if (arr1) {
        arr1[0] = 100;
        printf("arr1[0] = %d\n", arr1[0]);
    }
    
    // 查看统计信息
    mempool_stats(pool);
    
    // 释放内存
    mempool_free(pool, arr1);
    mempool_free(pool, str1);
    
    mempool_stats(pool);
    
    // 销毁内存池
    mempool_destroy(&pool);
}
```

### 5. 配置管理系统

#### 场景：应用程序配置
使用 const 指针保护配置数据。

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 配置项
typedef struct {
    const char *key;      // 键（只读）
    const char *value;    // 值（只读）
} ConfigItem;

// 配置管理器
typedef struct {
    ConfigItem **items;   // 二级指针：配置项数组
    int count;
    int capacity;
    const char *config_file;  // 配置文件路径（只读）
} ConfigManager;

// 创建配置管理器
ConfigManager* config_create(const char *filename) {
    ConfigManager *cfg = malloc(sizeof(ConfigManager));
    if (!cfg) return NULL;
    
    cfg->items = malloc(10 * sizeof(ConfigItem*));
    if (!cfg->items) {
        free(cfg);
        return NULL;
    }
    
    *(const char**)&cfg->config_file = strdup(filename);
    cfg->count = 0;
    cfg->capacity = 10;
    
    return cfg;
}

// 设置配置项
int config_set(ConfigManager *cfg, const char *key, const char *value) {
    if (!cfg || !key || !value) return -1;
    
    // 查找是否已存在
    for (int i = 0; i < cfg->count; i++) {
        if (strcmp(cfg->items[i]->key, key) == 0) {
            // 更新值
            free((void*)cfg->items[i]->value);
            *(const char**)&cfg->items[i]->value = strdup(value);
            return 0;
        }
    }
    
    // 扩容
    if (cfg->count >= cfg->capacity) {
        int new_capacity = cfg->capacity * 2;
        ConfigItem **new_items = realloc(cfg->items, 
                                        new_capacity * sizeof(ConfigItem*));
        if (!new_items) return -1;
        cfg->items = new_items;
        cfg->capacity = new_capacity;
    }
    
    // 添加新项
    ConfigItem *item = malloc(sizeof(ConfigItem));
    if (!item) return -1;
    
    *(const char**)&item->key = strdup(key);
    *(const char**)&item->value = strdup(value);
    
    cfg->items[cfg->count++] = item;
    return 0;
}

// 获取配置项（返回 const 指针，防止修改）
const char* config_get(const ConfigManager *cfg, const char *key) {
    if (!cfg || !key) return NULL;
    
    for (int i = 0; i < cfg->count; i++) {
        if (strcmp(cfg->items[i]->key, key) == 0) {
            return cfg->items[i]->value;
        }
    }
    
    return NULL;  // 未找到
}

// 获取配置项（带默认值）
const char* config_get_or_default(const ConfigManager *cfg, const char *key,
                                 const char *default_value) {
    const char *value = config_get(cfg, key);
    return value ? value : default_value;
}

// 打印所有配置
void config_print(const ConfigManager *cfg) {
    if (!cfg) return;
    
    printf("Configuration from '%s':\n", cfg->config_file);
    for (int i = 0; i < cfg->count; i++) {
        printf("  %s = %s\n", cfg->items[i]->key, cfg->items[i]->value);
    }
}

// 使用示例
void demo_config_system() {
    ConfigManager *cfg = config_create("app.conf");
    
    // 设置配置
    config_set(cfg, "server.host", "localhost");
    config_set(cfg, "server.port", "8080");
    config_set(cfg, "database.url", "postgresql://localhost/mydb");
    config_set(cfg, "logging.level", "INFO");
    
    // 读取配置
    const char *host = config_get(cfg, "server.host");
    const char *port = config_get(cfg, "server.port");
    const char *timeout = config_get_or_default(cfg, "server.timeout", "30");
    
    printf("Server: %s:%s (timeout=%s)\n", host, port, timeout);
    
    // 打印所有配置
    config_print(cfg);
}
```

### 工程最佳实践总结

#### 1. 使用 const 的原则
- **API 设计**：明确函数是否修改参数
- **数据保护**：防止意外修改重要数据
- **编译器优化**：帮助编译器优化代码
- **文档作用**：const 本身就是文档

#### 2. 使用二级指针的场景
- **修改指针本身**：函数需要改变指针指向
- **动态数组**：管理可变大小的指针数组
- **链表操作**：简化插入和删除操作
- **资源管理**：实现引用计数和智能指针

#### 3. 代码组织建议
```c
// 良好的函数签名设计

// 1. 只读参数使用 const
size_t calculate_total(const int *numbers, size_t count);

// 2. 输出参数使用二级指针
int get_user_info(const char *username, UserInfo **out_info);

// 3. 修改参数不使用 const
void update_statistics(Statistics *stats, int new_value);

// 4. 返回只读数据使用 const
const char* get_error_message(int error_code);

// 5. 函数指针用于回调
void process_async(const char *data, void (*callback)(const char *result));
```

#### 4. 内存管理模式
```c
// 模式 1：创建-销毁配对
Object* object_create(const char *name);
void object_destroy(Object **obj);

// 模式 2：引用计数
void object_ref(Object *obj);
void object_unref(Object **obj);

// 模式 3：资源获取即初始化（RAII 风格）
typedef struct {
    void *resource;
    void (*cleanup)(void*);
} Guard;

Guard* guard_create(void *resource, void (*cleanup)(void*));
void guard_release(Guard **guard);
```

## 野指针与悬空指针
- **野指针**：未初始化或指向已释放的内存
- **悬空指针**：指向的对象已销毁
- **防范**：初始化为 `NULL`、释放后置 `NULL`、检查有效性

```c
int *p = NULL;
if (p) {
    *p = 10;  // 安全检查
}
```
