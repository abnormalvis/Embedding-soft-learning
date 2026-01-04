# 队列（Queue）详细指南

## 目录
- [1. 队列的定义与作用](#1-队列的定义与作用)
- [2. 应用场景](#2-应用场景)
- [3. 优劣分析](#3-优劣分析)
- [4. 实现方式](#4-实现方式)
- [5. 代码实现](#5-代码实现)
- [6. 复杂度分析](#6-复杂度分析)
- [7. 实际应用示例](#7-实际应用示例)

## 1. 队列的定义与作用

### 1.1 定义
队列（Queue）是一种**先进先出**（FIFO - First In First Out）的线性数据结构。元素从队尾插入，从队首删除。

### 1.2 基本概念
- **队首（Front/Head）**: 允许删除的一端
- **队尾（Rear/Tail）**: 允许插入的一端
- **空队列**: 不含任何元素的队列
- **队满**: 队列中元素达到最大容量

### 1.3 核心操作
```c
enqueue(element)  // 入队：在队尾插入元素
dequeue()         // 出队：删除并返回队首元素
front()/peek()    // 查看队首元素但不删除
isEmpty()         // 判断队列是否为空
isFull()          // 判断队列是否已满（顺序队列）
size()            // 返回队列中元素个数
```

### 1.4 主要作用
1. **任务调度**: 按照到达顺序处理任务
2. **缓冲区管理**: 暂存数据流
3. **广度优先搜索**: 层序遍历算法的核心
4. **异步处理**: 消息队列、事件队列

## 2. 应用场景

### 2.1 系统级应用

| 应用 | 描述 | 示例 |
|------|------|------|
| **进程调度** | 操作系统按FIFO调度进程 | CPU时间片轮转 |
| **打印队列** | 多个打印任务按顺序执行 | 办公室共享打印机 |
| **IO缓冲** | 缓冲输入输出数据流 | 键盘缓冲区、网络数据包 |
| **消息队列** | 进程间通信 | RabbitMQ, Kafka |

### 2.2 算法应用
1. **广度优先搜索（BFS）**: 图的层序遍历
2. **二叉树层序遍历**: 逐层访问树节点
3. **缓存淘汰算法**: FIFO页面置换
4. **最短路径**: BFS求无权图最短路径
5. **拓扑排序**: Kahn算法使用队列

### 2.3 实际应用场景
- **操作系统**: 进程调度、磁盘调度
- **网络**: 路由器的数据包队列
- **生活场景**: 银行排队、售票系统
- **游戏开发**: 回合制游戏、动画帧队列
- **Web服务**: 请求队列、任务队列
- **多媒体**: 音视频播放缓冲

## 3. 优劣分析

### 3.1 优点

#### 3.1.1 公平性
- **FIFO保证**: 先到先服务，公平合理
- **无饥饿**: 每个元素都能被处理到
- **顺序性**: 保持元素到达的顺序

#### 3.1.2 性能优势
- **O(1)操作**: 入队和出队都是常数时间
- **高效处理**: 适合流式数据处理
- **并发友好**: 生产者-消费者模式

#### 3.1.3 应用优势
- **自然建模**: 符合很多实际场景
- **简单直观**: 容易理解和实现
- **广泛支持**: 各种编程语言都有实现

### 3.2 缺点

#### 3.2.1 功能限制
- **访问受限**: 只能访问队首和队尾
- **查找效率低**: 需要出队才能访问中间元素（O(n)）
- **不支持优先级**: 无法插队或调整顺序（需要优先队列）

#### 3.2.2 顺序队列的问题
- **假溢出**: front后移导致空间浪费
- **固定大小**: 需要预先分配空间
- **需要循环实现**: 普通顺序队列效率低

#### 3.2.3 链式队列的问题
- **指针开销**: 每个元素需要额外指针
- **内存分配**: 频繁malloc/free影响性能
- **缓存不友好**: 节点分散在内存中

### 3.3 队列类型对比

| 特性 | 顺序队列 | 循环队列 | 链式队列 |
|------|----------|----------|----------|
| 存储方式 | 连续数组 | 连续数组（循环） | 链表 |
| 空间利用 | 差（假溢出） | 好 | 好 |
| 实现复杂度 | 简单 | 中等 | 稍复杂 |
| 扩容能力 | 差 | 差 | 动态增长 |
| 内存局部性 | 好 | 好 | 差 |
| 推荐使用 | 不推荐 | 固定大小场景 | 动态场景 |

## 4. 实现方式

### 4.1 普通顺序队列（不推荐）

#### 4.1.1 结构设计
```c
#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];
    int front;  // 队首指针
    int rear;   // 队尾指针
} SeqQueue;
```

#### 4.1.2 问题
- front和rear只增不减，导致假溢出
- 即使队列实际上不满，也无法继续入队

### 4.2 循环队列（推荐的顺序实现）

#### 4.2.1 结构设计
```c
#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];
    int front;  // 队首指针
    int rear;   // 队尾指针（指向最后一个元素的下一个位置）
} CircularQueue;
```

#### 4.2.2 核心思想
- 使用取模运算实现循环：`(rear + 1) % MAX_SIZE`
- 队满条件：`(rear + 1) % MAX_SIZE == front`
- 队空条件：`front == rear`
- 牺牲一个存储单元来区分队满和队空

#### 4.2.3 优点
- 解决假溢出问题
- 充分利用数组空间
- 所有操作仍为O(1)

### 4.3 链式队列

#### 4.3.1 结构设计
```c
typedef struct QueueNode {
    int data;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front;  // 队首指针
    QueueNode *rear;   // 队尾指针
    int size;          // 队列大小
} LinkQueue;
```

#### 4.3.2 特点
- 使用单链表实现
- 需要front和rear两个指针
- 无队满问题
- 适合元素数量不确定的场景

## 5. 代码实现

### 5.1 循环队列完整实现

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_SIZE 5

// 循环队列结构定义
typedef struct {
    int data[MAX_SIZE];
    int front;  // 队首索引
    int rear;   // 队尾索引（指向最后一个元素的下一个位置）
} CircularQueue;

// 初始化队列
void initQueue(CircularQueue *q) {
    q->front = 0;
    q->rear = 0;
}

// 判断队列是否为空
bool isEmpty(CircularQueue *q) {
    return q->front == q->rear;
}

// 判断队列是否已满
bool isFull(CircularQueue *q) {
    return (q->rear + 1) % MAX_SIZE == q->front;
}

// 入队
bool enqueue(CircularQueue *q, int element) {
    if (isFull(q)) {
        printf("Error: Queue is full\n");
        return false;
    }
    q->data[q->rear] = element;
    q->rear = (q->rear + 1) % MAX_SIZE;
    return true;
}

// 出队
bool dequeue(CircularQueue *q, int *element) {
    if (isEmpty(q)) {
        printf("Error: Queue is empty\n");
        return false;
    }
    *element = q->data[q->front];
    q->front = (q->front + 1) % MAX_SIZE;
    return true;
}

// 查看队首元素
bool front(CircularQueue *q, int *element) {
    if (isEmpty(q)) {
        printf("Error: Queue is empty\n");
        return false;
    }
    *element = q->data[q->front];
    return true;
}

// 获取队列大小
int size(CircularQueue *q) {
    return (q->rear - q->front + MAX_SIZE) % MAX_SIZE;
}

// 清空队列
void clear(CircularQueue *q) {
    q->front = 0;
    q->rear = 0;
}

// 打印队列中所有元素
void printQueue(CircularQueue *q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return;
    }
    printf("Queue (front to rear): ");
    int i = q->front;
    while (i != q->rear) {
        printf("%d ", q->data[i]);
        i = (i + 1) % MAX_SIZE;
    }
    printf("\n");
}
```

### 5.2 链式队列完整实现

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// 队列节点定义
typedef struct QueueNode {
    int data;
    struct QueueNode *next;
} QueueNode;

// 队列结构定义
typedef struct {
    QueueNode *front;  // 队首指针
    QueueNode *rear;   // 队尾指针
    int size;          // 队列大小
} LinkQueue;

// 初始化队列
void initQueue(LinkQueue *q) {
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}

// 判断队列是否为空
bool isEmpty(LinkQueue *q) {
    return q->front == NULL;
}

// 入队
bool enqueue(LinkQueue *q, int element) {
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    if (newNode == NULL) {
        printf("Error: Memory allocation failed\n");
        return false;
    }
    
    newNode->data = element;
    newNode->next = NULL;
    
    if (isEmpty(q)) {
        // 队列为空，新节点既是队首也是队尾
        q->front = newNode;
        q->rear = newNode;
    } else {
        // 队列不为空，在队尾添加
        q->rear->next = newNode;
        q->rear = newNode;
    }
    
    q->size++;
    return true;
}

// 出队
bool dequeue(LinkQueue *q, int *element) {
    if (isEmpty(q)) {
        printf("Error: Queue is empty\n");
        return false;
    }
    
    QueueNode *temp = q->front;
    *element = temp->data;
    q->front = q->front->next;
    
    // 如果队列变空，rear也要置空
    if (q->front == NULL) {
        q->rear = NULL;
    }
    
    free(temp);
    q->size--;
    return true;
}

// 查看队首元素
bool front(LinkQueue *q, int *element) {
    if (isEmpty(q)) {
        printf("Error: Queue is empty\n");
        return false;
    }
    *element = q->front->data;
    return true;
}

// 获取队列大小
int size(LinkQueue *q) {
    return q->size;
}

// 清空队列
void clear(LinkQueue *q) {
    int temp;
    while (!isEmpty(q)) {
        dequeue(q, &temp);
    }
}

// 销毁队列
void destroyQueue(LinkQueue *q) {
    clear(q);
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}

// 打印队列中所有元素
void printQueue(LinkQueue *q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return;
    }
    
    printf("Queue (front to rear): ");
    QueueNode *current = q->front;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}
```

### 5.3 双端队列（Deque）实现

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_SIZE 10

// 双端队列结构定义
typedef struct {
    int data[MAX_SIZE];
    int front;
    int rear;
    int size;
} Deque;

// 初始化双端队列
void initDeque(Deque *dq) {
    dq->front = 0;
    dq->rear = 0;
    dq->size = 0;
}

// 判断队列是否为空
bool isEmpty(Deque *dq) {
    return dq->size == 0;
}

// 判断队列是否已满
bool isFull(Deque *dq) {
    return dq->size == MAX_SIZE;
}

// 从队首插入
bool pushFront(Deque *dq, int element) {
    if (isFull(dq)) {
        printf("Error: Deque is full\n");
        return false;
    }
    dq->front = (dq->front - 1 + MAX_SIZE) % MAX_SIZE;
    dq->data[dq->front] = element;
    dq->size++;
    return true;
}

// 从队尾插入
bool pushRear(Deque *dq, int element) {
    if (isFull(dq)) {
        printf("Error: Deque is full\n");
        return false;
    }
    dq->data[dq->rear] = element;
    dq->rear = (dq->rear + 1) % MAX_SIZE;
    dq->size++;
    return true;
}

// 从队首删除
bool popFront(Deque *dq, int *element) {
    if (isEmpty(dq)) {
        printf("Error: Deque is empty\n");
        return false;
    }
    *element = dq->data[dq->front];
    dq->front = (dq->front + 1) % MAX_SIZE;
    dq->size--;
    return true;
}

// 从队尾删除
bool popRear(Deque *dq, int *element) {
    if (isEmpty(dq)) {
        printf("Error: Deque is empty\n");
        return false;
    }
    dq->rear = (dq->rear - 1 + MAX_SIZE) % MAX_SIZE;
    *element = dq->data[dq->rear];
    dq->size--;
    return true;
}

// 查看队首元素
bool getFront(Deque *dq, int *element) {
    if (isEmpty(dq)) {
        printf("Error: Deque is empty\n");
        return false;
    }
    *element = dq->data[dq->front];
    return true;
}

// 查看队尾元素
bool getRear(Deque *dq, int *element) {
    if (isEmpty(dq)) {
        printf("Error: Deque is empty\n");
        return false;
    }
    int rearIndex = (dq->rear - 1 + MAX_SIZE) % MAX_SIZE;
    *element = dq->data[rearIndex];
    return true;
}
```

## 6. 复杂度分析

### 6.1 时间复杂度

| 操作 | 循环队列 | 链式队列 | 说明 |
|------|----------|----------|------|
| enqueue() | O(1) | O(1) | 直接在队尾插入 |
| dequeue() | O(1) | O(1) | 直接从队首删除 |
| front() | O(1) | O(1) | 直接访问队首 |
| isEmpty() | O(1) | O(1) | 检查指针或索引 |
| search() | O(n) | O(n) | 需要遍历整个队列 |

### 6.2 空间复杂度

| 实现方式 | 空间复杂度 | 说明 |
|----------|------------|------|
| 循环队列 | O(n) | n为预分配的最大容量 |
| 链式队列 | O(n) | n为实际元素个数，每个节点额外需要指针空间 |

## 7. 实际应用示例

### 7.1 二叉树层序遍历（BFS）

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// 二叉树节点定义
typedef struct TreeNode {
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

// 队列节点定义（存储树节点指针）
typedef struct QueueNode {
    TreeNode *treeNode;
    struct QueueNode *next;
} QueueNode;

// 队列结构定义
typedef struct {
    QueueNode *front;
    QueueNode *rear;
} Queue;

// 初始化队列
void initQueue(Queue *q) {
    q->front = NULL;
    q->rear = NULL;
}

// 判断队列是否为空
bool isEmpty(Queue *q) {
    return q->front == NULL;
}

// 入队
void enqueue(Queue *q, TreeNode *node) {
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    newNode->treeNode = node;
    newNode->next = NULL;
    
    if (isEmpty(q)) {
        q->front = newNode;
        q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

// 出队
TreeNode* dequeue(Queue *q) {
    if (isEmpty(q)) return NULL;
    
    QueueNode *temp = q->front;
    TreeNode *treeNode = temp->treeNode;
    q->front = q->front->next;
    
    if (q->front == NULL) {
        q->rear = NULL;
    }
    
    free(temp);
    return treeNode;
}

// 创建新的树节点
TreeNode* createNode(int data) {
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// 层序遍历（广度优先搜索）
void levelOrderTraversal(TreeNode *root) {
    if (root == NULL) {
        printf("Tree is empty\n");
        return;
    }
    
    Queue q;
    initQueue(&q);
    enqueue(&q, root);
    
    printf("Level Order Traversal: ");
    while (!isEmpty(&q)) {
        TreeNode *current = dequeue(&q);
        printf("%d ", current->data);
        
        // 将左右子节点加入队列
        if (current->left != NULL) {
            enqueue(&q, current->left);
        }
        if (current->right != NULL) {
            enqueue(&q, current->right);
        }
    }
    printf("\n");
}

// 测试函数
void testLevelOrder() {
    // 创建示例树:
    //       1
    //      / \
    //     2   3
    //    / \   \
    //   4   5   6
    
    TreeNode *root = createNode(1);
    root->left = createNode(2);
    root->right = createNode(3);
    root->left->left = createNode(4);
    root->left->right = createNode(5);
    root->right->right = createNode(6);
    
    printf("=== 二叉树层序遍历测试 ===\n");
    levelOrderTraversal(root);
}
```

### 7.2 进程调度模拟（时间片轮转）

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// 进程结构定义
typedef struct {
    int pid;              // 进程ID
    char name[20];        // 进程名称
    int burstTime;        // 需要的CPU时间
    int remainingTime;    // 剩余执行时间
} Process;

// 队列节点定义
typedef struct QueueNode {
    Process process;
    struct QueueNode *next;
} QueueNode;

// 队列结构定义
typedef struct {
    QueueNode *front;
    QueueNode *rear;
    int size;
} ProcessQueue;

// 初始化队列
void initQueue(ProcessQueue *q) {
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}

// 判断队列是否为空
bool isEmpty(ProcessQueue *q) {
    return q->front == NULL;
}

// 入队
void enqueue(ProcessQueue *q, Process proc) {
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    newNode->process = proc;
    newNode->next = NULL;
    
    if (isEmpty(q)) {
        q->front = newNode;
        q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    q->size++;
}

// 出队
bool dequeue(ProcessQueue *q, Process *proc) {
    if (isEmpty(q)) return false;
    
    QueueNode *temp = q->front;
    *proc = temp->process;
    q->front = q->front->next;
    
    if (q->front == NULL) {
        q->rear = NULL;
    }
    
    free(temp);
    q->size--;
    return true;
}

// 时间片轮转调度算法
void roundRobinScheduling(Process processes[], int n, int timeQuantum) {
    ProcessQueue readyQueue;
    initQueue(&readyQueue);
    
    // 将所有进程加入就绪队列
    for (int i = 0; i < n; i++) {
        processes[i].remainingTime = processes[i].burstTime;
        enqueue(&readyQueue, processes[i]);
    }
    
    int currentTime = 0;
    printf("=== 时间片轮转调度（时间片 = %d） ===\n", timeQuantum);
    
    while (!isEmpty(&readyQueue)) {
        Process current;
        dequeue(&readyQueue, &current);
        
        // 计算本次执行时间
        int executeTime = (current.remainingTime > timeQuantum) 
                          ? timeQuantum 
                          : current.remainingTime;
        
        printf("Time %d: Process %d (%s) executing for %d units\n",
               currentTime, current.pid, current.name, executeTime);
        
        currentTime += executeTime;
        current.remainingTime -= executeTime;
        
        // 如果进程未完成，重新加入队列
        if (current.remainingTime > 0) {
            enqueue(&readyQueue, current);
        } else {
            printf("       Process %d (%s) completed at time %d\n",
                   current.pid, current.name, currentTime);
        }
    }
    
    printf("All processes completed at time %d\n", currentTime);
}

// 测试函数
void testProcessScheduling() {
    Process processes[] = {
        {1, "P1", 10, 0},
        {2, "P2", 4, 0},
        {3, "P3", 6, 0},
        {4, "P4", 8, 0}
    };
    
    int n = sizeof(processes) / sizeof(processes[0]);
    int timeQuantum = 3;
    
    roundRobinScheduling(processes, n, timeQuantum);
}
```

### 7.3 滑动窗口最大值（双端队列应用）

```c
#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 1000

// 双端队列（存储数组索引）
typedef struct {
    int data[MAX_SIZE];
    int front;
    int rear;
    int size;
} Deque;

void initDeque(Deque *dq) {
    dq->front = 0;
    dq->rear = 0;
    dq->size = 0;
}

bool isEmpty(Deque *dq) {
    return dq->size == 0;
}

bool isFull(Deque *dq) {
    return dq->size == MAX_SIZE;
}

void pushRear(Deque *dq, int value) {
    if (isFull(dq)) return;
    dq->data[dq->rear] = value;
    dq->rear = (dq->rear + 1) % MAX_SIZE;
    dq->size++;
}

void popFront(Deque *dq) {
    if (isEmpty(dq)) return;
    dq->front = (dq->front + 1) % MAX_SIZE;
    dq->size--;
}

void popRear(Deque *dq) {
    if (isEmpty(dq)) return;
    dq->rear = (dq->rear - 1 + MAX_SIZE) % MAX_SIZE;
    dq->size--;
}

int getFront(Deque *dq) {
    if (isEmpty(dq)) return -1;
    return dq->data[dq->front];
}

int getRear(Deque *dq) {
    if (isEmpty(dq)) return -1;
    int rearIndex = (dq->rear - 1 + MAX_SIZE) % MAX_SIZE;
    return dq->data[rearIndex];
}

// 滑动窗口最大值
// 例如: nums = [1,3,-1,-3,5,3,6,7], k = 3
// 输出: [3,3,5,5,6,7]
void maxSlidingWindow(int nums[], int n, int k, int result[]) {
    Deque dq;
    initDeque(&dq);
    int resultIndex = 0;
    
    for (int i = 0; i < n; i++) {
        // 移除不在窗口内的元素
        if (!isEmpty(&dq) && getFront(&dq) < i - k + 1) {
            popFront(&dq);
        }
        
        // 移除所有小于当前元素的索引（从队尾）
        while (!isEmpty(&dq) && nums[getRear(&dq)] < nums[i]) {
            popRear(&dq);
        }
        
        // 将当前元素索引加入队尾
        pushRear(&dq, i);
        
        // 窗口形成后，记录最大值
        if (i >= k - 1) {
            result[resultIndex++] = nums[getFront(&dq)];
        }
    }
}

// 测试函数
void testSlidingWindow() {
    int nums[] = {1, 3, -1, -3, 5, 3, 6, 7};
    int n = sizeof(nums) / sizeof(nums[0]);
    int k = 3;
    int result[n - k + 1];
    
    printf("\n=== 滑动窗口最大值测试 ===\n");
    printf("Array: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", nums[i]);
    }
    printf("\nWindow size: %d\n", k);
    
    maxSlidingWindow(nums, n, k, result);
    
    printf("Sliding window maximums: ");
    for (int i = 0; i < n - k + 1; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");
}
```

### 7.4 生产者-消费者问题

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define BUFFER_SIZE 5

// 循环队列作为缓冲区
typedef struct {
    int data[BUFFER_SIZE];
    int front;
    int rear;
} Buffer;

void initBuffer(Buffer *buf) {
    buf->front = 0;
    buf->rear = 0;
}

bool isBufferEmpty(Buffer *buf) {
    return buf->front == buf->rear;
}

bool isBufferFull(Buffer *buf) {
    return (buf->rear + 1) % BUFFER_SIZE == buf->front;
}

bool produce(Buffer *buf, int item) {
    if (isBufferFull(buf)) {
        return false;
    }
    buf->data[buf->rear] = item;
    buf->rear = (buf->rear + 1) % BUFFER_SIZE;
    return true;
}

bool consume(Buffer *buf, int *item) {
    if (isBufferEmpty(buf)) {
        return false;
    }
    *item = buf->data[buf->front];
    buf->front = (buf->front + 1) % BUFFER_SIZE;
    return true;
}

// 模拟生产者-消费者
void simulateProducerConsumer() {
    Buffer buffer;
    initBuffer(&buffer);
    
    printf("=== 生产者-消费者模拟 ===\n");
    printf("缓冲区大小: %d\n\n", BUFFER_SIZE);
    
    int produced = 0;
    int consumed = 0;
    int totalItems = 10;
    
    while (consumed < totalItems) {
        // 生产者尝试生产
        if (produced < totalItems && !isBufferFull(&buffer)) {
            produced++;
            produce(&buffer, produced);
            printf("生产者: 生产了项目 %d (缓冲区项目数: %d)\n", 
                   produced, 
                   (buffer.rear - buffer.front + BUFFER_SIZE) % BUFFER_SIZE);
        } else if (produced < totalItems) {
            printf("生产者: 缓冲区已满，等待...\n");
        }
        
        // 消费者尝试消费
        int item;
        if (!isBufferEmpty(&buffer)) {
            consume(&buffer, &item);
            consumed++;
            printf("消费者: 消费了项目 %d (缓冲区项目数: %d)\n", 
                   item, 
                   (buffer.rear - buffer.front + BUFFER_SIZE) % BUFFER_SIZE);
        } else if (produced < totalItems) {
            printf("消费者: 缓冲区为空，等待...\n");
        }
        
        printf("\n");
        usleep(500000);  // 暂停0.5秒
    }
    
    printf("所有项目已生产和消费完成\n");
}
```

## 8. 总结

### 8.1 何时使用队列
- 需要先进先出的处理顺序
- 任务调度和资源分配
- 广度优先搜索
- 缓冲区管理
- 异步数据传输

### 8.2 选择建议

#### 循环队列适用于：
- 元素数量可预测且相对固定
- 需要高性能和缓存友好性
- 实时系统，避免动态内存分配
- 嵌入式系统

#### 链式队列适用于：
- 元素数量不可预测
- 需要动态增长
- 不在意指针的额外空间开销
- 通用场景

#### 双端队列适用于：
- 需要在两端都能插入和删除
- 滑动窗口问题
- 回文检测
- 工作窃取算法

### 8.3 关键要点
1. 队列是FIFO结构，保证公平性
2. 循环队列解决顺序队列的假溢出问题
3. 所有基本操作都是O(1)时间复杂度
4. 队列是BFS算法的核心数据结构
5. 在操作系统、网络、并发编程中广泛应用
6. 双端队列提供了更大的灵活性

### 8.4 常见面试问题
1. 如何用两个栈实现一个队列？
2. 如何用队列实现栈？
3. 循环队列为什么要牺牲一个存储单元？
4. 如何判断循环队列的队满和队空？
5. 队列和栈的区别？应用场景有什么不同？
