# 二叉树（Binary Tree）详细指南

## 目录
- [1. 二叉树的定义与作用](#1-二叉树的定义与作用)
- [2. 应用场景](#2-应用场景)
- [3. 优劣分析](#3-优劣分析)
- [4. 二叉树的类型](#4-二叉树的类型)
- [5. 二叉树的性质](#5-二叉树的性质)
- [6. 实现方式](#6-实现方式)
- [7. 代码实现](#7-代码实现)
- [8. 遍历算法](#8-遍历算法)
- [9. 实际应用示例](#9-实际应用示例)

## 1. 二叉树的定义与作用

### 1.1 定义
二叉树（Binary Tree）是一种树形数据结构，其中每个节点最多有两个子节点，通常称为**左子节点**和**右子节点**。

### 1.2 基本概念

#### 1.2.1 节点术语
- **根节点（Root）**: 树的顶端节点，没有父节点
- **叶子节点（Leaf）**: 没有子节点的节点
- **内部节点（Internal Node）**: 至少有一个子节点的节点
- **父节点（Parent）**: 有子节点的节点
- **子节点（Child）**: 节点的直接后继
- **兄弟节点（Sibling）**: 具有相同父节点的节点

#### 1.2.2 树的属性
- **深度（Depth）**: 从根到节点的边数
- **高度（Height）**: 从节点到最深叶子的边数
- **层级（Level）**: 节点的深度 + 1
- **度（Degree）**: 节点的子节点数量

### 1.3 核心操作
```c
insert()           // 插入节点
delete()           // 删除节点
search()           // 查找节点
traverse()         // 遍历树
getHeight()        // 获取树的高度
getSize()          // 获取节点数量
isBalanced()       // 判断是否平衡
```

### 1.4 主要作用
1. **层次化表示**: 表达具有层次关系的数据
2. **快速查找**: 某些二叉树（如BST）支持高效查找
3. **表达式树**: 表示和计算数学表达式
4. **决策树**: AI和机器学习中的决策模型
5. **文件系统**: 目录结构的组织

## 2. 应用场景

### 2.1 数据结构基础

| 应用 | 描述 | 示例 |
|------|------|------|
| **二叉搜索树** | 支持高效查找、插入、删除 | BST, AVL树, 红黑树 |
| **堆** | 优先队列的实现 | 最大堆、最小堆 |
| **哈夫曼树** | 数据压缩 | 文件压缩算法 |
| **线段树** | 区间查询和更新 | 竞赛编程 |

### 2.2 算法应用
1. **表达式求值**: 前缀、中缀、后缀表达式树
2. **语法分析**: 编译器的语法树（AST）
3. **决策树**: 机器学习分类算法
4. **路径查找**: 游戏AI、迷宫求解
5. **数据库索引**: B树、B+树

### 2.3 实际应用
- **文件系统**: 目录树结构
- **DOM树**: HTML文档对象模型
- **组织结构图**: 公司层级关系
- **XML/JSON解析**: 树形数据表示
- **游戏开发**: 场景图、四叉树
- **网络路由**: 路由表的树形组织

## 3. 优劣分析

### 3.1 优点

#### 3.1.1 结构优势
- **层次清晰**: 天然表达层次关系
- **递归友好**: 结构本身适合递归处理
- **灵活性高**: 可以表达多种逻辑关系

#### 3.1.2 性能优势（特定类型）
- **BST查找**: 平均O(log n)，优于线性结构
- **堆操作**: O(log n)的插入和删除最值
- **平衡树**: 保证最坏情况的性能

#### 3.1.3 应用优势
- **自然建模**: 很多问题天然是树形结构
- **分治策略**: 便于应用分治算法
- **空间效率**: 某些场景下比哈希表更省空间

### 3.2 缺点

#### 3.2.1 性能限制
- **非平衡退化**: 普通二叉树可能退化成链表（O(n)）
- **缓存不友好**: 节点分散在内存中
- **空间开销**: 每个节点需要2个指针
- **遍历开销**: 某些操作需要遍历整棵树

#### 3.2.2 实现复杂度
- **平衡维护**: 平衡二叉树的旋转操作复杂
- **删除复杂**: 删除节点的情况较多
- **内存管理**: 需要手动管理节点内存（C/C++）

#### 3.2.3 使用限制
- **不支持随机访问**: 无法像数组一样O(1)访问
- **顺序遍历慢**: 遍历整棵树需要O(n)
- **不适合频繁插入**: 某些场景下不如哈希表

### 3.3 不同类型对比

| 树类型 | 查找 | 插入 | 删除 | 平衡性 | 实现难度 |
|--------|------|------|------|--------|----------|
| 普通二叉树 | O(n) | O(1) | O(n) | 无保证 | 简单 |
| 二叉搜索树 | O(n) | O(n) | O(n) | 无保证 | 中等 |
| AVL树 | O(log n) | O(log n) | O(log n) | 严格平衡 | 复杂 |
| 红黑树 | O(log n) | O(log n) | O(log n) | 近似平衡 | 很复杂 |
| 堆 | O(n) | O(log n) | O(log n) | 完全树 | 中等 |

## 4. 二叉树的类型

### 4.1 满二叉树（Full Binary Tree）
- **定义**: 每个节点要么有0个子节点，要么有2个子节点
- **特点**: 没有只有一个子节点的节点

```
        1
       / \
      2   3
     / \
    4   5
```

### 4.2 完全二叉树（Complete Binary Tree）
- **定义**: 除最后一层外，其他层都是满的，最后一层从左到右连续
- **特点**: 可以用数组高效存储
- **应用**: 堆的实现

```
        1
       / \
      2   3
     / \  /
    4  5 6
```

### 4.3 完美二叉树（Perfect Binary Tree）
- **定义**: 所有内部节点都有两个子节点，所有叶子在同一层
- **特点**: 节点数 = 2^h - 1（h为高度）

```
        1
       / \
      2   3
     / \ / \
    4  5 6  7
```

### 4.4 平衡二叉树（Balanced Binary Tree）
- **定义**: 任意节点的左右子树高度差不超过1
- **应用**: AVL树

### 4.5 二叉搜索树（Binary Search Tree）
- **定义**: 左子树所有节点 < 根节点 < 右子树所有节点
- **应用**: 快速查找和排序

### 4.6 退化二叉树（Degenerate/Skewed Tree）
- **定义**: 每个节点只有一个子节点，退化成链表
- **性能**: 所有操作退化为O(n)

```
    1
     \
      2
       \
        3
         \
          4
```

## 5. 二叉树的性质

### 5.1 基本性质

1. **性质1**: 第i层最多有 $2^{i-1}$ 个节点（i ≥ 1）

2. **性质2**: 深度为k的二叉树最多有 $2^k - 1$ 个节点（k ≥ 1）

3. **性质3**: 对于任何非空二叉树，如果叶子节点数为 $n_0$，度为2的节点数为 $n_2$，则：
   $$n_0 = n_2 + 1$$

4. **性质4**: n个节点的完全二叉树的高度为 $\lfloor \log_2 n \rfloor + 1$

### 5.2 完全二叉树的数组存储

对于完全二叉树，如果节点存储在数组中（下标从0开始）：
- 父节点索引为i，则：
  - 左子节点索引：$2i + 1$
  - 右子节点索引：$2i + 2$
- 子节点索引为i，则：
  - 父节点索引：$\lfloor (i-1)/2 \rfloor$

## 6. 实现方式

### 6.1 链式存储（常用）

#### 6.1.1 节点结构
```c
typedef struct TreeNode {
    int data;                // 数据域
    struct TreeNode *left;   // 左子节点指针
    struct TreeNode *right;  // 右子节点指针
} TreeNode;
```

#### 6.1.2 特点
- 灵活，适合各种形状的二叉树
- 需要额外的指针空间
- 支持动态增长

### 6.2 数组存储（适合完全二叉树）

#### 6.2.1 存储方式
```c
#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];  // 数据数组
    int size;            // 当前节点数
} ArrayTree;
```

#### 6.2.2 特点
- 无需存储指针，节省空间
- 只适合完全二叉树
- 访问父子节点通过计算索引

### 6.3 三叉链表（带父指针）

```c
typedef struct TreeNode {
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
    struct TreeNode *parent;  // 父节点指针
} TreeNode;
```

#### 特点
- 便于向上遍历
- 额外空间开销

## 7. 代码实现

### 7.1 基本二叉树实现

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

// 创建新节点
TreeNode* createNode(int data) {
    TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));
    if (newNode == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// 插入节点（层序方式，用于构建完全二叉树）
TreeNode* insertLevelOrder(int arr[], int i, int n) {
    TreeNode *root = NULL;
    
    if (i < n) {
        root = createNode(arr[i]);
        root->left = insertLevelOrder(arr, 2 * i + 1, n);
        root->right = insertLevelOrder(arr, 2 * i + 2, n);
    }
    
    return root;
}

// 查找节点
TreeNode* search(TreeNode *root, int key) {
    if (root == NULL) {
        return NULL;
    }
    
    if (root->data == key) {
        return root;
    }
    
    // 在左子树查找
    TreeNode *leftResult = search(root->left, key);
    if (leftResult != NULL) {
        return leftResult;
    }
    
    // 在右子树查找
    return search(root->right, key);
}

// 获取树的高度
int getHeight(TreeNode *root) {
    if (root == NULL) {
        return 0;
    }
    
    int leftHeight = getHeight(root->left);
    int rightHeight = getHeight(root->right);
    
    return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}

// 获取节点数量
int getSize(TreeNode *root) {
    if (root == NULL) {
        return 0;
    }
    
    return 1 + getSize(root->left) + getSize(root->right);
}

// 获取叶子节点数量
int getLeafCount(TreeNode *root) {
    if (root == NULL) {
        return 0;
    }
    
    if (root->left == NULL && root->right == NULL) {
        return 1;
    }
    
    return getLeafCount(root->left) + getLeafCount(root->right);
}

// 判断是否为平衡二叉树
bool isBalancedHelper(TreeNode *root, int *height) {
    if (root == NULL) {
        *height = 0;
        return true;
    }
    
    int leftHeight, rightHeight;
    bool leftBalanced = isBalancedHelper(root->left, &leftHeight);
    bool rightBalanced = isBalancedHelper(root->right, &rightHeight);
    
    *height = (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
    
    if (!leftBalanced || !rightBalanced) {
        return false;
    }
    
    return abs(leftHeight - rightHeight) <= 1;
}

bool isBalanced(TreeNode *root) {
    int height;
    return isBalancedHelper(root, &height);
}

// 判断两棵树是否相同
bool isSameTree(TreeNode *p, TreeNode *q) {
    if (p == NULL && q == NULL) {
        return true;
    }
    
    if (p == NULL || q == NULL) {
        return false;
    }
    
    return (p->data == q->data) &&
           isSameTree(p->left, q->left) &&
           isSameTree(p->right, q->right);
}

// 镜像翻转二叉树
TreeNode* invertTree(TreeNode *root) {
    if (root == NULL) {
        return NULL;
    }
    
    // 交换左右子树
    TreeNode *temp = root->left;
    root->left = root->right;
    root->right = temp;
    
    // 递归翻转左右子树
    invertTree(root->left);
    invertTree(root->right);
    
    return root;
}

// 销毁二叉树（后序遍历方式）
void destroyTree(TreeNode *root) {
    if (root == NULL) {
        return;
    }
    
    destroyTree(root->left);
    destroyTree(root->right);
    free(root);
}
```

### 7.2 数组实现的完全二叉树

```c
#include <stdio.h>
#include <stdbool.h>

#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];
    int size;
} ArrayTree;

// 初始化树
void initTree(ArrayTree *tree) {
    tree->size = 0;
}

// 判断树是否为空
bool isEmpty(ArrayTree *tree) {
    return tree->size == 0;
}

// 判断树是否已满
bool isFull(ArrayTree *tree) {
    return tree->size == MAX_SIZE;
}

// 插入节点（作为完全二叉树的下一个位置）
bool insert(ArrayTree *tree, int data) {
    if (isFull(tree)) {
        printf("Tree is full\n");
        return false;
    }
    
    tree->data[tree->size++] = data;
    return true;
}

// 获取父节点索引
int getParentIndex(int index) {
    if (index == 0) return -1;
    return (index - 1) / 2;
}

// 获取左子节点索引
int getLeftChildIndex(int index) {
    return 2 * index + 1;
}

// 获取右子节点索引
int getRightChildIndex(int index) {
    return 2 * index + 2;
}

// 获取节点值
int getValue(ArrayTree *tree, int index) {
    if (index < 0 || index >= tree->size) {
        return -1;  // 错误值
    }
    return tree->data[index];
}

// 获取树的高度
int getHeight(ArrayTree *tree) {
    if (tree->size == 0) return 0;
    
    int height = 0;
    int count = 0;
    int levelSize = 1;
    
    while (count < tree->size) {
        height++;
        count += levelSize;
        levelSize *= 2;
    }
    
    return height;
}

// 层序打印
void printLevelOrder(ArrayTree *tree) {
    if (isEmpty(tree)) {
        printf("Tree is empty\n");
        return;
    }
    
    printf("Level Order: ");
    for (int i = 0; i < tree->size; i++) {
        printf("%d ", tree->data[i]);
    }
    printf("\n");
}
```

## 8. 遍历算法

### 8.1 深度优先遍历（DFS）

#### 8.1.1 前序遍历（Pre-order）
访问顺序：**根 → 左 → 右**

```c
// 递归实现
void preorderTraversal(TreeNode *root) {
    if (root == NULL) return;
    
    printf("%d ", root->data);      // 访问根
    preorderTraversal(root->left);  // 遍历左子树
    preorderTraversal(root->right); // 遍历右子树
}

// 非递归实现（使用栈）
void preorderIterative(TreeNode *root) {
    if (root == NULL) return;
    
    TreeNode *stack[100];
    int top = -1;
    
    stack[++top] = root;
    
    while (top >= 0) {
        TreeNode *node = stack[top--];
        printf("%d ", node->data);
        
        // 先压右子树，再压左子树（栈是后进先出）
        if (node->right != NULL) {
            stack[++top] = node->right;
        }
        if (node->left != NULL) {
            stack[++top] = node->left;
        }
    }
}
```

#### 8.1.2 中序遍历（In-order）
访问顺序：**左 → 根 → 右**

```c
// 递归实现
void inorderTraversal(TreeNode *root) {
    if (root == NULL) return;
    
    inorderTraversal(root->left);   // 遍历左子树
    printf("%d ", root->data);       // 访问根
    inorderTraversal(root->right);  // 遍历右子树
}

// 非递归实现
void inorderIterative(TreeNode *root) {
    TreeNode *stack[100];
    int top = -1;
    TreeNode *current = root;
    
    while (current != NULL || top >= 0) {
        // 一直往左走，将所有左节点入栈
        while (current != NULL) {
            stack[++top] = current;
            current = current->left;
        }
        
        // 弹出栈顶节点并访问
        current = stack[top--];
        printf("%d ", current->data);
        
        // 转向右子树
        current = current->right;
    }
}
```

#### 8.1.3 后序遍历（Post-order）
访问顺序：**左 → 右 → 根**

```c
// 递归实现
void postorderTraversal(TreeNode *root) {
    if (root == NULL) return;
    
    postorderTraversal(root->left);  // 遍历左子树
    postorderTraversal(root->right); // 遍历右子树
    printf("%d ", root->data);        // 访问根
}

// 非递归实现（使用两个栈）
void postorderIterative(TreeNode *root) {
    if (root == NULL) return;
    
    TreeNode *stack1[100], *stack2[100];
    int top1 = -1, top2 = -1;
    
    stack1[++top1] = root;
    
    while (top1 >= 0) {
        TreeNode *node = stack1[top1--];
        stack2[++top2] = node;
        
        if (node->left != NULL) {
            stack1[++top1] = node->left;
        }
        if (node->right != NULL) {
            stack1[++top1] = node->right;
        }
    }
    
    while (top2 >= 0) {
        printf("%d ", stack2[top2--]->data);
    }
}
```

### 8.2 广度优先遍历（BFS）

#### 8.2.1 层序遍历（Level-order）
逐层访问，从左到右

```c
#include <stdlib.h>

typedef struct QueueNode {
    TreeNode *treeNode;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front;
    QueueNode *rear;
} Queue;

void initQueue(Queue *q) {
    q->front = NULL;
    q->rear = NULL;
}

void enqueue(Queue *q, TreeNode *node) {
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    newNode->treeNode = node;
    newNode->next = NULL;
    
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

TreeNode* dequeue(Queue *q) {
    if (q->front == NULL) return NULL;
    
    QueueNode *temp = q->front;
    TreeNode *treeNode = temp->treeNode;
    q->front = q->front->next;
    
    if (q->front == NULL) {
        q->rear = NULL;
    }
    
    free(temp);
    return treeNode;
}

bool isQueueEmpty(Queue *q) {
    return q->front == NULL;
}

// 层序遍历
void levelOrderTraversal(TreeNode *root) {
    if (root == NULL) return;
    
    Queue q;
    initQueue(&q);
    enqueue(&q, root);
    
    while (!isQueueEmpty(&q)) {
        TreeNode *node = dequeue(&q);
        printf("%d ", node->data);
        
        if (node->left != NULL) {
            enqueue(&q, node->left);
        }
        if (node->right != NULL) {
            enqueue(&q, node->right);
        }
    }
}

// 按层打印（每层一行）
void printByLevel(TreeNode *root) {
    if (root == NULL) return;
    
    Queue q;
    initQueue(&q);
    enqueue(&q, root);
    
    while (!isQueueEmpty(&q)) {
        int levelSize = 0;
        QueueNode *temp = q.front;
        
        // 计算当前层的节点数
        while (temp != NULL) {
            levelSize++;
            temp = temp->next;
        }
        
        // 处理当前层的所有节点
        for (int i = 0; i < levelSize; i++) {
            TreeNode *node = dequeue(&q);
            printf("%d ", node->data);
            
            if (node->left != NULL) {
                enqueue(&q, node->left);
            }
            if (node->right != NULL) {
                enqueue(&q, node->right);
            }
        }
        printf("\n");
    }
}
```

### 8.3 遍历对比

| 遍历方式 | 访问顺序 | 应用场景 | 时间复杂度 | 空间复杂度 |
|----------|----------|----------|------------|------------|
| 前序 | 根-左-右 | 复制树、前缀表达式 | O(n) | O(h) |
| 中序 | 左-根-右 | BST排序、中缀表达式 | O(n) | O(h) |
| 后序 | 左-右-根 | 删除树、后缀表达式 | O(n) | O(h) |
| 层序 | 逐层从左到右 | 层次关系、最短路径 | O(n) | O(w) |

注：h为树的高度，w为树的最大宽度

## 9. 实际应用示例

### 9.1 表达式树

```c
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// 创建表达式树节点
TreeNode* createExprNode(char c) {
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    node->data = c;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// 判断是否为运算符
bool isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

// 从后缀表达式构建表达式树
TreeNode* buildExpressionTree(const char *postfix) {
    TreeNode *stack[100];
    int top = -1;
    
    for (int i = 0; postfix[i] != '\0'; i++) {
        char c = postfix[i];
        
        if (c == ' ') continue;
        
        if (isOperator(c)) {
            // 运算符节点
            TreeNode *node = createExprNode(c);
            node->right = stack[top--];  // 右操作数
            node->left = stack[top--];   // 左操作数
            stack[++top] = node;
        } else {
            // 操作数节点
            TreeNode *node = createExprNode(c);
            stack[++top] = node;
        }
    }
    
    return stack[top];
}

// 计算表达式树的值
int evaluateExpressionTree(TreeNode *root) {
    if (root == NULL) return 0;
    
    // 叶子节点是操作数
    if (root->left == NULL && root->right == NULL) {
        return root->data - '0';  // 转换字符到数字
    }
    
    // 递归计算左右子树
    int leftVal = evaluateExpressionTree(root->left);
    int rightVal = evaluateExpressionTree(root->right);
    
    // 根据运算符计算
    switch (root->data) {
        case '+': return leftVal + rightVal;
        case '-': return leftVal - rightVal;
        case '*': return leftVal * rightVal;
        case '/': return leftVal / rightVal;
    }
    
    return 0;
}

// 打印中缀表达式（带括号）
void printInfixExpression(TreeNode *root) {
    if (root == NULL) return;
    
    if (root->left != NULL) {
        printf("(");
        printInfixExpression(root->left);
    }
    
    printf("%c", root->data);
    
    if (root->right != NULL) {
        printInfixExpression(root->right);
        printf(")");
    }
}

// 测试表达式树
void testExpressionTree() {
    // 后缀表达式: "53+82-*" 表示 (5+3)*(8-2) = 48
    const char *postfix = "53+82-*";
    
    printf("=== 表达式树测试 ===\n");
    printf("后缀表达式: %s\n", postfix);
    
    TreeNode *root = buildExpressionTree(postfix);
    
    printf("中缀表达式: ");
    printInfixExpression(root);
    printf("\n");
    
    int result = evaluateExpressionTree(root);
    printf("计算结果: %d\n", result);
}
```

### 9.2 路径和问题

```c
#include <stdbool.h>

// 判断是否存在从根到叶子的路径，路径和等于目标值
bool hasPathSum(TreeNode *root, int targetSum) {
    if (root == NULL) {
        return false;
    }
    
    // 叶子节点
    if (root->left == NULL && root->right == NULL) {
        return root->data == targetSum;
    }
    
    // 递归检查左右子树
    int remainingSum = targetSum - root->data;
    return hasPathSum(root->left, remainingSum) || 
           hasPathSum(root->right, remainingSum);
}

// 打印所有路径和等于目标值的路径
void printPathsWithSum(TreeNode *root, int targetSum, int path[], int pathLen) {
    if (root == NULL) return;
    
    // 将当前节点加入路径
    path[pathLen] = root->data;
    pathLen++;
    
    // 如果是叶子节点且路径和等于目标值
    if (root->left == NULL && root->right == NULL) {
        int sum = 0;
        for (int i = 0; i < pathLen; i++) {
            sum += path[i];
        }
        
        if (sum == targetSum) {
            printf("Path: ");
            for (int i = 0; i < pathLen; i++) {
                printf("%d ", path[i]);
            }
            printf("(Sum = %d)\n", sum);
        }
    }
    
    // 递归遍历左右子树
    printPathsWithSum(root->left, targetSum, path, pathLen);
    printPathsWithSum(root->right, targetSum, path, pathLen);
}

// 最大路径和
int maxPathSumHelper(TreeNode *root, int *maxSum) {
    if (root == NULL) return 0;
    
    // 递归计算左右子树的最大路径和（负数取0）
    int leftMax = maxPathSumHelper(root->left, maxSum);
    int rightMax = maxPathSumHelper(root->right, maxSum);
    
    leftMax = leftMax > 0 ? leftMax : 0;
    rightMax = rightMax > 0 ? rightMax : 0;
    
    // 更新全局最大值（经过当前节点的路径）
    int currentMax = root->data + leftMax + rightMax;
    if (currentMax > *maxSum) {
        *maxSum = currentMax;
    }
    
    // 返回经过当前节点的单边最大路径
    return root->data + (leftMax > rightMax ? leftMax : rightMax);
}

int maxPathSum(TreeNode *root) {
    int maxSum = -2147483648;  // INT_MIN
    maxPathSumHelper(root, &maxSum);
    return maxSum;
}
```

### 9.3 二叉树的序列化与反序列化

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 序列化二叉树（前序遍历）
void serializeHelper(TreeNode *root, char *str, int *index) {
    if (root == NULL) {
        str[(*index)++] = '#';
        str[(*index)++] = ',';
        return;
    }
    
    // 将节点值转换为字符串
    char buffer[20];
    sprintf(buffer, "%d,", root->data);
    int len = strlen(buffer);
    strcpy(str + *index, buffer);
    *index += len;
    
    serializeHelper(root->left, str, index);
    serializeHelper(root->right, str, index);
}

char* serialize(TreeNode *root) {
    char *result = (char *)malloc(10000 * sizeof(char));
    int index = 0;
    serializeHelper(root, result, &index);
    result[index] = '\0';
    return result;
}

// 反序列化二叉树
TreeNode* deserializeHelper(char **str) {
    if (**str == '\0') return NULL;
    
    if (**str == '#') {
        (*str) += 2;  // 跳过 '#' 和 ','
        return NULL;
    }
    
    // 解析节点值
    int val = 0;
    int sign = 1;
    
    if (**str == '-') {
        sign = -1;
        (*str)++;
    }
    
    while (**str != ',' && **str != '\0') {
        val = val * 10 + (**str - '0');
        (*str)++;
    }
    (*str)++;  // 跳过 ','
    
    TreeNode *node = createNode(sign * val);
    node->left = deserializeHelper(str);
    node->right = deserializeHelper(str);
    
    return node;
}

TreeNode* deserialize(char *data) {
    if (data == NULL || data[0] == '\0') return NULL;
    return deserializeHelper(&data);
}
```

## 10. 总结

### 10.1 何时使用二叉树
- 需要表达层次关系的数据
- 需要快速查找、插入、删除（BST）
- 实现优先队列（堆）
- 表达式求值和语法分析
- 决策问题建模

### 10.2 选择建议
- **普通二叉树**: 简单的层次结构表示
- **二叉搜索树**: 需要快速查找和排序
- **平衡二叉树**: 需要保证最坏情况性能
- **完全二叉树**: 可以用数组高效存储（堆）

### 10.3 关键要点
1. 二叉树是递归数据结构，很多操作用递归实现简洁
2. 遍历方式的选择取决于具体应用场景
3. 平衡性对性能影响巨大
4. 完全二叉树可以用数组高效存储
5. 掌握各种遍历的递归和非递归实现

### 10.4 性能优化建议
1. 对于查找密集的场景，使用平衡二叉搜索树
2. 对于完全二叉树，考虑数组存储
3. 缓存树的高度等属性，避免重复计算
4. 尾递归优化或改用迭代实现
5. 考虑使用三叉链表便于向上遍历
