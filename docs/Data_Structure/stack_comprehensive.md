# 栈（Stack）详细指南

## 目录
- [1. 栈的定义与作用](#1-栈的定义与作用)
- [2. 应用场景](#2-应用场景)
- [3. 优劣分析](#3-优劣分析)
- [4. 实现方式](#4-实现方式)
- [5. 代码实现](#5-代码实现)
- [6. 复杂度分析](#6-复杂度分析)
- [7. 实际应用示例](#7-实际应用示例)

## 1. 栈的定义与作用

### 1.1 定义
栈（Stack）是一种**后进先出**（LIFO - Last In First Out）的线性数据结构。只能在栈顶进行插入和删除操作。

### 1.2 基本概念
- **栈顶（Top）**: 允许插入和删除的一端
- **栈底（Bottom）**: 固定的一端，不允许插入和删除
- **空栈**: 不含任何元素的栈
- **栈满**: 栈中元素达到最大容量（仅顺序栈）

### 1.3 核心操作
```c
push(element)    // 入栈：将元素压入栈顶
pop()            // 出栈：删除并返回栈顶元素
peek()/top()     // 查看栈顶元素但不删除
isEmpty()        // 判断栈是否为空
isFull()         // 判断栈是否已满（顺序栈）
size()           // 返回栈中元素个数
```

### 1.4 主要作用
1. **临时存储**: 保存临时数据，需要时按相反顺序取出
2. **回溯机制**: 支持撤销操作和历史记录
3. **递归实现**: 函数调用栈保存执行上下文
4. **表达式求值**: 中缀、后缀表达式转换与计算

## 2. 应用场景

### 2.1 系统级应用
| 应用 | 描述 | 示例 |
|------|------|------|
| **函数调用栈** | 保存函数调用的返回地址和局部变量 | 递归函数执行、异常处理 |
| **中断处理** | 保存中断前的程序状态 | 操作系统中断服务 |
| **内存管理** | 管理自动变量的生命周期 | 局部变量的分配与释放 |

### 2.2 算法应用
1. **深度优先搜索（DFS）**: 使用栈实现图的遍历
2. **括号匹配**: 检查括号是否正确配对
3. **表达式求值**: 
   - 中缀转后缀（调度场算法）
   - 后缀表达式计算
4. **迷宫求解**: 回溯算法的实现
5. **汉诺塔问题**: 递归问题的迭代实现

### 2.3 实际应用
- **浏览器历史记录**: 前进/后退功能
- **文本编辑器**: 撤销（Undo）/重做（Redo）
- **编译器**: 语法分析、括号匹配
- **表达式计算器**: 算术表达式求值
- **路径查找**: 迷宫、游戏AI

## 3. 优劣分析

### 3.1 优点

#### 3.1.1 性能优势
- **O(1)时间复杂度**: 入栈和出栈操作时间复杂度为常数级
- **空间局部性好**: 连续的内存访问（顺序栈）
- **缓存友好**: 数据访问模式可预测

#### 3.1.2 实现优势
- **简单直观**: 实现和使用都很简单
- **内存可控**: 可以精确控制内存使用（顺序栈）
- **无需复杂指针**: 顺序栈只需一个数组和一个索引

#### 3.1.3 逻辑优势
- **符合自然思维**: 后进先出符合很多实际场景
- **支持回溯**: 天然支持撤销和回退操作
- **递归转迭代**: 可以用栈模拟递归调用

### 3.2 缺点

#### 3.2.1 功能限制
- **访问受限**: 只能访问栈顶元素，无法随机访问
- **查找效率低**: 需要依次出栈才能找到特定元素（O(n)）
- **不支持中间插入**: 无法在栈中间插入或删除元素

#### 3.2.2 实现限制（顺序栈）
- **固定大小**: 需要预先分配空间，可能浪费或不足
- **栈溢出风险**: 元素过多时可能导致栈溢出
- **扩容成本高**: 需要复制所有元素到新空间

#### 3.2.3 实现限制（链式栈）
- **指针开销**: 每个元素需要额外的指针空间
- **内存分配**: 频繁的动态内存分配可能影响性能
- **缓存不友好**: 节点可能分散在内存中

### 3.3 对比表

| 特性 | 顺序栈 | 链式栈 |
|------|--------|--------|
| 存储方式 | 连续内存（数组） | 分散内存（链表） |
| 空间效率 | 可能浪费空间 | 按需分配 |
| 时间效率 | push/pop O(1) | push/pop O(1) |
| 扩容能力 | 需要复制 | 动态增长 |
| 内存局部性 | 好 | 差 |
| 实现复杂度 | 简单 | 稍复杂 |

## 4. 实现方式

### 4.1 顺序栈（基于数组）

#### 4.1.1 结构设计
```c
#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];  // 存储元素的数组
    int top;             // 栈顶指针，指向栈顶元素
} SeqStack;
```

#### 4.1.2 特点
- 使用一维数组存储元素
- 用top指针标记栈顶位置
- top = -1 表示空栈
- top = MAX_SIZE - 1 表示栈满

### 4.2 链式栈（基于链表）

#### 4.2.1 结构设计
```c
typedef struct StackNode {
    int data;                // 数据域
    struct StackNode *next;  // 指针域，指向下一个节点
} StackNode, *LinkStack;
```

#### 4.2.2 特点
- 使用单链表实现
- 链表头作为栈顶（便于插入删除）
- 无栈满问题，受限于系统内存
- 需要动态分配内存

## 5. 代码实现

### 5.1 顺序栈完整实现

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_SIZE 100

// 栈结构定义
typedef struct {
    int data[MAX_SIZE];
    int top;
} SeqStack;

// 初始化栈
void initStack(SeqStack *s) {
    s->top = -1;
}

// 判断栈是否为空
bool isEmpty(SeqStack *s) {
    return s->top == -1;
}

// 判断栈是否已满
bool isFull(SeqStack *s) {
    return s->top == MAX_SIZE - 1;
}

// 入栈
bool push(SeqStack *s, int element) {
    if (isFull(s)) {
        printf("Error: Stack overflow\n");
        return false;
    }
    s->data[++s->top] = element;
    return true;
}

// 出栈
bool pop(SeqStack *s, int *element) {
    if (isEmpty(s)) {
        printf("Error: Stack underflow\n");
        return false;
    }
    *element = s->data[s->top--];
    return true;
}

// 查看栈顶元素
bool peek(SeqStack *s, int *element) {
    if (isEmpty(s)) {
        printf("Error: Stack is empty\n");
        return false;
    }
    *element = s->data[s->top];
    return true;
}

// 获取栈的大小
int size(SeqStack *s) {
    return s->top + 1;
}

// 清空栈
void clear(SeqStack *s) {
    s->top = -1;
}

// 打印栈中所有元素（从栈底到栈顶）
void printStack(SeqStack *s) {
    if (isEmpty(s)) {
        printf("Stack is empty\n");
        return;
    }
    printf("Stack (bottom to top): ");
    for (int i = 0; i <= s->top; i++) {
        printf("%d ", s->data[i]);
    }
    printf("\n");
}
```

### 5.2 链式栈完整实现

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// 栈节点定义
typedef struct StackNode {
    int data;
    struct StackNode *next;
} StackNode;

// 栈结构定义
typedef struct {
    StackNode *top;  // 栈顶指针
    int size;        // 栈的大小
} LinkStack;

// 初始化栈
void initStack(LinkStack *s) {
    s->top = NULL;
    s->size = 0;
}

// 判断栈是否为空
bool isEmpty(LinkStack *s) {
    return s->top == NULL;
}

// 入栈
bool push(LinkStack *s, int element) {
    StackNode *newNode = (StackNode *)malloc(sizeof(StackNode));
    if (newNode == NULL) {
        printf("Error: Memory allocation failed\n");
        return false;
    }
    newNode->data = element;
    newNode->next = s->top;
    s->top = newNode;
    s->size++;
    return true;
}

// 出栈
bool pop(LinkStack *s, int *element) {
    if (isEmpty(s)) {
        printf("Error: Stack underflow\n");
        return false;
    }
    StackNode *temp = s->top;
    *element = temp->data;
    s->top = temp->next;
    free(temp);
    s->size--;
    return true;
}

// 查看栈顶元素
bool peek(LinkStack *s, int *element) {
    if (isEmpty(s)) {
        printf("Error: Stack is empty\n");
        return false;
    }
    *element = s->top->data;
    return true;
}

// 获取栈的大小
int size(LinkStack *s) {
    return s->size;
}

// 清空栈
void clear(LinkStack *s) {
    int temp;
    while (!isEmpty(s)) {
        pop(s, &temp);
    }
}

// 销毁栈
void destroyStack(LinkStack *s) {
    clear(s);
    s->top = NULL;
    s->size = 0;
}

// 打印栈中所有元素
void printStack(LinkStack *s) {
    if (isEmpty(s)) {
        printf("Stack is empty\n");
        return;
    }
    printf("Stack (top to bottom): ");
    StackNode *current = s->top;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}
```

## 6. 复杂度分析

### 6.1 时间复杂度

| 操作 | 顺序栈 | 链式栈 | 说明 |
|------|--------|--------|------|
| push() | O(1) | O(1) | 直接在栈顶操作 |
| pop() | O(1) | O(1) | 直接移除栈顶元素 |
| peek() | O(1) | O(1) | 直接访问栈顶 |
| isEmpty() | O(1) | O(1) | 检查指针或索引 |
| search() | O(n) | O(n) | 需要遍历整个栈 |

### 6.2 空间复杂度

| 实现方式 | 空间复杂度 | 说明 |
|----------|------------|------|
| 顺序栈 | O(n) | n为预分配的最大容量 |
| 链式栈 | O(n) | n为实际元素个数，每个节点额外需要指针空间 |

## 7. 实际应用示例

### 7.1 括号匹配检查

```c
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 100

typedef struct {
    char data[MAX_SIZE];
    int top;
} CharStack;

void initStack(CharStack *s) {
    s->top = -1;
}

bool isEmpty(CharStack *s) {
    return s->top == -1;
}

bool push(CharStack *s, char ch) {
    if (s->top == MAX_SIZE - 1) return false;
    s->data[++s->top] = ch;
    return true;
}

bool pop(CharStack *s, char *ch) {
    if (isEmpty(s)) return false;
    *ch = s->data[s->top--];
    return true;
}

bool isMatchingPair(char open, char close) {
    return (open == '(' && close == ')') ||
           (open == '[' && close == ']') ||
           (open == '{' && close == '}');
}

bool checkBrackets(const char *expression) {
    CharStack stack;
    initStack(&stack);
    
    for (int i = 0; expression[i] != '\0'; i++) {
        char ch = expression[i];
        
        // 如果是左括号，入栈
        if (ch == '(' || ch == '[' || ch == '{') {
            push(&stack, ch);
        }
        // 如果是右括号，检查匹配
        else if (ch == ')' || ch == ']' || ch == '}') {
            if (isEmpty(&stack)) {
                return false;  // 右括号多余
            }
            char top;
            pop(&stack, &top);
            if (!isMatchingPair(top, ch)) {
                return false;  // 括号不匹配
            }
        }
    }
    
    return isEmpty(&stack);  // 栈为空说明所有括号都匹配
}

// 测试函数
void testBracketMatching() {
    const char *test_cases[] = {
        "()",
        "()[]{}",
        "{[()]}",
        "([)]",
        "(()",
        "())",
        "{[}]",
        ""
    };
    
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    
    printf("=== 括号匹配测试 ===\n");
    for (int i = 0; i < num_cases; i++) {
        bool result = checkBrackets(test_cases[i]);
        printf("Expression: \"%s\" -> %s\n", 
               test_cases[i], 
               result ? "Valid" : "Invalid");
    }
}
```

### 7.2 后缀表达式求值

```c
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];
    int top;
} IntStack;

void initStack(IntStack *s) {
    s->top = -1;
}

bool isEmpty(IntStack *s) {
    return s->top == -1;
}

bool push(IntStack *s, int value) {
    if (s->top == MAX_SIZE - 1) return false;
    s->data[++s->top] = value;
    return true;
}

bool pop(IntStack *s, int *value) {
    if (isEmpty(s)) return false;
    *value = s->data[s->top--];
    return true;
}

// 计算后缀表达式
// 例如: "5 3 + 2 *" = (5 + 3) * 2 = 16
int evaluatePostfix(const char *expression) {
    IntStack stack;
    initStack(&stack);
    
    int i = 0;
    while (expression[i] != '\0') {
        // 跳过空格
        if (expression[i] == ' ') {
            i++;
            continue;
        }
        
        // 如果是数字，读取完整的数字并入栈
        if (isdigit(expression[i])) {
            int num = 0;
            while (isdigit(expression[i])) {
                num = num * 10 + (expression[i] - '0');
                i++;
            }
            push(&stack, num);
        }
        // 如果是运算符，弹出两个数进行计算
        else if (expression[i] == '+' || expression[i] == '-' ||
                 expression[i] == '*' || expression[i] == '/') {
            int operand2, operand1;
            pop(&stack, &operand2);
            pop(&stack, &operand1);
            
            int result;
            switch (expression[i]) {
                case '+': result = operand1 + operand2; break;
                case '-': result = operand1 - operand2; break;
                case '*': result = operand1 * operand2; break;
                case '/': result = operand1 / operand2; break;
            }
            push(&stack, result);
            i++;
        }
        else {
            i++;
        }
    }
    
    int finalResult;
    pop(&stack, &finalResult);
    return finalResult;
}

// 测试函数
void testPostfixEvaluation() {
    const char *expressions[] = {
        "5 3 +",           // 8
        "5 3 + 2 *",       // 16
        "15 7 1 1 + - / 3 * 2 1 1 + + -",  // 5
        "5 1 2 + 4 * + 3 -"  // 14
    };
    
    int num_expressions = sizeof(expressions) / sizeof(expressions[0]);
    
    printf("\n=== 后缀表达式求值测试 ===\n");
    for (int i = 0; i < num_expressions; i++) {
        int result = evaluatePostfix(expressions[i]);
        printf("Expression: \"%s\" = %d\n", expressions[i], result);
    }
}
```

### 7.3 迷宫求解（DFS）

```c
#include <stdio.h>
#include <stdbool.h>

#define ROWS 5
#define COLS 5
#define MAX_SIZE 100

typedef struct {
    int x, y;
} Position;

typedef struct {
    Position data[MAX_SIZE];
    int top;
} PosStack;

void initStack(PosStack *s) {
    s->top = -1;
}

bool isEmpty(PosStack *s) {
    return s->top == -1;
}

bool push(PosStack *s, Position pos) {
    if (s->top == MAX_SIZE - 1) return false;
    s->data[++s->top] = pos;
    return true;
}

bool pop(PosStack *s, Position *pos) {
    if (isEmpty(s)) return false;
    *pos = s->data[s->top--];
    return true;
}

// 迷宫求解
// 0表示通路，1表示墙壁
bool solveMaze(int maze[ROWS][COLS], Position start, Position end) {
    PosStack path;
    bool visited[ROWS][COLS] = {false};
    int directions[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};  // 上下左右
    
    initStack(&path);
    push(&path, start);
    visited[start.x][start.y] = true;
    
    while (!isEmpty(&path)) {
        Position current = path.data[path.top];  // 查看栈顶
        
        // 找到出口
        if (current.x == end.x && current.y == end.y) {
            printf("找到路径:\n");
            for (int i = 0; i <= path.top; i++) {
                printf("(%d, %d) ", path.data[i].x, path.data[i].y);
            }
            printf("\n");
            return true;
        }
        
        // 尝试四个方向
        bool found_next = false;
        for (int i = 0; i < 4; i++) {
            int nx = current.x + directions[i][0];
            int ny = current.y + directions[i][1];
            
            // 检查是否可以移动到该位置
            if (nx >= 0 && nx < ROWS && ny >= 0 && ny < COLS &&
                maze[nx][ny] == 0 && !visited[nx][ny]) {
                Position next = {nx, ny};
                push(&path, next);
                visited[nx][ny] = true;
                found_next = true;
                break;
            }
        }
        
        // 如果没有找到可走的路，回溯
        if (!found_next) {
            Position temp;
            pop(&path, &temp);
        }
    }
    
    printf("没有找到路径\n");
    return false;
}

// 测试迷宫求解
void testMazeSolver() {
    int maze[ROWS][COLS] = {
        {0, 1, 0, 0, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0}
    };
    
    Position start = {0, 0};
    Position end = {4, 4};
    
    printf("\n=== 迷宫求解测试 ===\n");
    printf("迷宫 (0=路径, 1=墙壁):\n");
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%d ", maze[i][j]);
        }
        printf("\n");
    }
    
    solveMaze(maze, start, end);
}
```

## 8. 总结

### 8.1 何时使用栈
- 需要后进先出的访问模式
- 实现回溯算法
- 深度优先搜索
- 表达式求值和语法分析
- 函数调用管理

### 8.2 选择建议
- **顺序栈**: 
  - 元素数量可预测
  - 需要高性能和缓存友好性
  - 简单场景，不需要频繁扩容
  
- **链式栈**:
  - 元素数量不可预测
  - 需要动态增长
  - 不在意指针的额外空间开销

### 8.3 关键要点
1. 栈是受限的线性结构，只能在一端操作
2. LIFO原则使得栈非常适合回溯和递归场景
3. 所有基本操作都是O(1)时间复杂度
4. 选择顺序栈还是链式栈取决于具体需求
5. 栈在编译器、操作系统、算法设计中有广泛应用
