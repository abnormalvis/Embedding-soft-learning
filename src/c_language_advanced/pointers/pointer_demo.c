#include <stdio.h>
#include <stdlib.h>

static void demo_basic_pointer(void)
{
    puts("=== Basic Pointer ===");
    int value = 42;
    int *ptr = &value;

    printf("value=%d address=%p\n", value, (void *)&value);
    printf("ptr=%p *ptr=%d\n", (void *)ptr, *ptr);

    *ptr = 100;
    printf("after *ptr=100, value=%d\n", value);
}

static void demo_pointer_arithmetic(void)
{
    puts("\n=== Pointer Arithmetic ===");
    int nums[] = {10, 20, 30, 40, 50};
    int *p = nums;

    for (int i = 0; i < 5; ++i)
    {
        printf("p+%d: address=%p value=%d\n", i, (void *)(p + i), *(p + i));
    }

    printf("p[2]=%d *(p+2)=%d\n", p[2], *(p + 2));
}

static void demo_pointer_to_struct(void)
{
    puts("\n=== Pointer to Struct ===");
    struct Point
    {
        int x, y;
    };

    struct Point pt = {10, 20};
    struct Point *ptr = &pt;

    printf("pt.x=%d pt.y=%d\n", pt.x, pt.y);
    ptr->x = 30;
    ptr->y = 40;
    printf("after modification: pt.x=%d pt.y=%d\n", pt.x, pt.y);
}

static void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

static void demo_function_with_pointer(void)
{
    puts("\n=== Function with Pointer ===");
    int x = 5, y = 10;
    printf("before swap: x=%d y=%d\n", x, y);
    swap(&x, &y);
    printf("after swap: x=%d y=%d\n", x, y);
}

typedef void (*callback_fn)(int);

static void process_value(int value, callback_fn cb)
{
    cb(value * 2);
}

static void print_callback(int x)
{
    printf("callback received: %d\n", x);
}

static void demo_function_pointer(void)
{
    puts("\n=== Function Pointer ===");
    process_value(21, print_callback);
}

static void demo_const_pointer(void)
{
    puts("\n=== Const Pointer ===");
    int x = 10, y = 20;

    const int *p1 = &x;
    printf("const int *p1: *p1=%d\n", *p1);
    // *p1 = 30;  // 错误：不能修改指向的内容
    p1 = &y; // 允许：可以修改指针
    printf("after p1=&y: *p1=%d\n", *p1);

    int *const p2 = &x;
    *p2 = 30; // 允许：可以修改指向的内容
    printf("int * const p2: *p2=%d\n", *p2);
    // p2 = &y;   // 错误：不能修改指针
}

static void demo_multi_level_pointer(void)
{
    puts("\n=== Multi-level Pointer ===");
    int value = 42;
    int *p = &value;
    int **pp = &p;

    printf("value=%d\n", value);
    printf("*p=%d\n", *p);
    printf("**pp=%d\n", **pp);

    **pp = 100;
    printf("after **pp=100, value=%d\n", value);
}

int main(void)
{
    demo_basic_pointer();
    demo_pointer_arithmetic();
    demo_pointer_to_struct();
    demo_function_with_pointer();
    demo_function_pointer();
    demo_const_pointer();
    demo_multi_level_pointer();
    return 0;
}
