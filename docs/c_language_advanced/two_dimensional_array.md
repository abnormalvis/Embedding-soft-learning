# 二维数组

## 语法基础
- 二维数组可视为数组的数组，按行优先顺序在内存中连续存储。
- 声明：`类型 数组名[行数][列数];`
- 访问元素：`array[i][j]` 表示第 `i` 行第 `j` 列。

```c
int matrix[3][4];  // 3 行 4 列
int grid[2][3] = {
    {1, 2, 3},
    {4, 5, 6}
};
```

## 内存剖析
- 二维数组在内存中是一维连续分布，按行依次存放。
- `matrix[0]` 是第 0 行的首地址，类型为 `int *`。
- `matrix` 可退化为 `int (*)[4]`，指向一个包含 4 个 `int` 的数组。

```c
int grid[2][3] = {{1,2,3}, {4,5,6}};
// 内存布局: [1][2][3][4][5][6]
printf("&grid[0][0]=%p &grid[1][0]=%p\n", &grid[0][0], &grid[1][0]);
// 地址差为 12 字节（3 个 int）
```

### 指针访问
- `grid[i][j]` 等价于 `*(grid[i] + j)` 或 `*((int *)grid + i * cols + j)`。
- 使用指针遍历时需要知道列数以计算偏移。

## 遍历方式
```c
int rows = 2, cols = 3;
for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
        printf("%d ", grid[i][j]);
    }
    putchar('\n');
}

// 指针方式
int *p = &grid[0][0];
for (int i = 0; i < rows * cols; ++i) {
    printf("%d ", p[i]);
}
```

## 传递给函数
- 必须指定除第一维外的所有维度大小。
- 或使用指针 + 手动计算偏移。

```c
void print_matrix(int arr[][3], int rows) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < 3; ++j) {
            printf("%d ", arr[i][j]);
        }
        putchar('\n');
    }
}

// 或用一维指针
void print_flat(int *arr, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            printf("%d ", arr[i * cols + j]);
        }
        putchar('\n');
    }
}
```

## 应用场景
- **矩阵运算**：图像处理、科学计算、线性代数。
- **游戏棋盘**：围棋、象棋、扫雷等网格布局。
- **查找表**：多维映射关系，如坐标转换。
- **动态规划**：DP 表通常使用二维数组存储中间结果。

## 动态二维数组
- 栈上二维数组大小固定，动态分配需要手动构造指针数组：

```c
int rows = 3, cols = 4;
int **matrix = malloc(rows * sizeof(int *));
for (int i = 0; i < rows; ++i) {
    matrix[i] = malloc(cols * sizeof(int));
}
// 使用 matrix[i][j]
// 释放时需逐行 free
for (int i = 0; i < rows; ++i) {
    free(matrix[i]);
}
free(matrix);
```

- 或一次性分配连续内存：

```c
int *data = malloc(rows * cols * sizeof(int));
// 访问 data[i * cols + j]
free(data);
```
