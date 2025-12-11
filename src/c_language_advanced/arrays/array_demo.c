#include <stdio.h>
#include <stdlib.h>

static void demo_one_dimensional(void) {
    puts("=== One Dimensional Array ===");
    int nums[5] = {10, 20, 30, 40, 50};
    
    printf("sizeof(nums)=%zu bytes\n", sizeof(nums));
    printf("address of nums=%p, nums[0]=%p\n", (void *)nums, (void *)&nums[0]);
    
    // 下标遍历
    printf("subscript access: ");
    for (size_t i = 0; i < 5; ++i) {
        printf("%d ", nums[i]);
    }
    putchar('\n');
    
    // 指针遍历
    printf("pointer access: ");
    for (int *p = nums; p < nums + 5; ++p) {
        printf("%d ", *p);
    }
    putchar('\n');
}

static void demo_two_dimensional(void) {
    puts("\n=== Two Dimensional Array ===");
    int matrix[3][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12}
    };
    
    printf("sizeof(matrix)=%zu bytes\n", sizeof(matrix));
    printf("address diff: &matrix[1][0] - &matrix[0][0] = %td bytes\n", 
           (char *)&matrix[1][0] - (char *)&matrix[0][0]);
    
    // 二维遍历
    puts("matrix content:");
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            printf("%3d ", matrix[i][j]);
        }
        putchar('\n');
    }
    
    // 一维指针访问
    int *flat = &matrix[0][0];
    printf("flat access: ");
    for (int i = 0; i < 12; ++i) {
        printf("%d ", flat[i]);
    }
    putchar('\n');
}

static void demo_dynamic_2d_array(void) {
    puts("\n=== Dynamic 2D Array ===");
    int rows = 3, cols = 4;
    
    // 方法1: 指针数组
    int **arr = malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; ++i) {
        arr[i] = malloc(cols * sizeof(int));
        for (int j = 0; j < cols; ++j) {
            arr[i][j] = i * cols + j;
        }
    }
    
    puts("dynamic array:");
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            printf("%3d ", arr[i][j]);
        }
        putchar('\n');
    }
    
    for (int i = 0; i < rows; ++i) {
        free(arr[i]);
    }
    free(arr);
    
    // 方法2: 连续内存
    int *flat = malloc(rows * cols * sizeof(int));
    for (int i = 0; i < rows * cols; ++i) {
        flat[i] = i;
    }
    
    puts("continuous memory:");
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            printf("%3d ", flat[i * cols + j]);
        }
        putchar('\n');
    }
    free(flat);
}

int main(void) {
    demo_one_dimensional();
    demo_two_dimensional();
    demo_dynamic_2d_array();
    return 0;
}
