#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 全局变量 - 数据段
int g_initialized = 100;

// 未初始化全局变量 - BSS段
int g_uninitialized;

// 静态全局变量 - 数据段
static int s_global = 200;

static void print_addresses(void) {
    // 局部变量 - 栈
    int local = 10;
    static int static_local = 20;  // 数据段
    
    // 动态分配 - 堆
    int *heap_ptr = malloc(sizeof(int));
    if (heap_ptr) {
        *heap_ptr = 30;
    }
    
    // 字符串字面量 - 代码段
    const char *str = "hello";
    
    puts("=== Memory Layout ===");
    printf("Code segment (string literal):   %p\n", (void *)str);
    printf("Data segment (g_initialized):    %p\n", (void *)&g_initialized);
    printf("Data segment (s_global):         %p\n", (void *)&s_global);
    printf("Data segment (static_local):     %p\n", (void *)&static_local);
    printf("BSS segment (g_uninitialized):   %p\n", (void *)&g_uninitialized);
    printf("Stack (local variable):          %p\n", (void *)&local);
    if (heap_ptr) {
        printf("Heap (malloc):                   %p\n", (void *)heap_ptr);
        free(heap_ptr);
    }
}

static void demo_stack(void) {
    puts("\n=== Stack Demo ===");
    int a = 1, b = 2, c = 3;
    
    printf("&a=%p &b=%p &c=%p\n", (void *)&a, (void *)&b, (void *)&c);
    printf("address difference: %td bytes\n", (char *)&a - (char *)&b);
}

static void demo_heap(void) {
    puts("\n=== Heap Demo ===");
    
    // 分配数组
    int *arr = malloc(5 * sizeof(int));
    if (!arr) {
        perror("malloc failed");
        return;
    }
    
    for (int i = 0; i < 5; ++i) {
        arr[i] = i * 10;
    }
    
    printf("heap array: ");
    for (int i = 0; i < 5; ++i) {
        printf("%d ", arr[i]);
    }
    putchar('\n');
    
    // 重新分配
    int *new_arr = realloc(arr, 8 * sizeof(int));
    if (!new_arr) {
        free(arr);
        perror("realloc failed");
        return;
    }
    arr = new_arr;
    
    for (int i = 5; i < 8; ++i) {
        arr[i] = i * 10;
    }
    
    printf("after realloc: ");
    for (int i = 0; i < 8; ++i) {
        printf("%d ", arr[i]);
    }
    putchar('\n');
    
    free(arr);
}

static void demo_data_segment(void) {
    puts("\n=== Data Segment ===");
    static int call_count = 0;
    ++call_count;
    
    printf("call_count=%d (address=%p)\n", call_count, (void *)&call_count);
    printf("g_initialized=%d g_uninitialized=%d\n", g_initialized, g_uninitialized);
}

static void recursive_stack_demo(int depth, int max_depth) {
    char buffer[100];
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "depth %d", depth);
    
    printf("%s stack address=%p\n", buffer, (void *)buffer);
    
    if (depth < max_depth) {
        recursive_stack_demo(depth + 1, max_depth);
    }
}

static void demo_stack_growth(void) {
    puts("\n=== Stack Growth Demo ===");
    recursive_stack_demo(0, 3);
}

int main(void) {
    print_addresses();
    demo_stack();
    demo_heap();
    demo_data_segment();
    demo_data_segment();  // 调用两次观察静态变量
    demo_stack_growth();
    return 0;
}
