#include <stdio.h>
#include <stdlib.h>

typedef int (*cmp_fn)(const void *, const void *);

static int compare_int(const void *lhs, const void *rhs) {
    int a = *(const int *)lhs;
    int b = *(const int *)rhs;
    return (a > b) - (a < b);
}

static void print_array(const int *values, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        printf("%d ", values[i]);
    }
    putchar('\n');
}

int main(void) {
    int data[] = { 7, 3, 9, 1, 5 };
    size_t count = sizeof data / sizeof data[0];
    puts("original:");
    print_array(data, count);
    qsort(data, count, sizeof(int), compare_int);
    puts("sorted:");
    print_array(data, count);
    return 0;
}
