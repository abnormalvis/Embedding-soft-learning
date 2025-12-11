#include <stdarg.h>
#include <stdio.h>
static int sum_ints(int count, ...)
{
    va_list args;
    va_start(args, count);
    int total = 0;
    for (int i = 0; i < count; ++i)
    {
        total += va_arg(args, int);
    }
    va_end(args);
    return total;
}
int main(void)
{
    int total = sum_ints(5, 1, 2, 3, 4, 5);
    printf("total=%d\n", total);
    return 0;
}
