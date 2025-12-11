#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int fill_values(int **out, size_t count)
{
    int *values = malloc(sizeof(int) * count);
    if (!values)
    {
        return -1;
    }
    memset(values, 0, sizeof(int) * count);
    for (size_t i = 0; i < count; ++i)
    {
        values[i] = (int)(i * 2);
    }
    *out = values;
    return 0;
}
int main(void)
{
    int *numbers = NULL;
    if (fill_values(&numbers, 5) != 0)
    {
        fputs("allocation failed\n", stderr);
        return 1;
    }
    int *expanded = realloc(numbers, sizeof(int) * 8);
    if (!expanded)
    {
        free(numbers);
        fputs("realloc failed\n", stderr);
        return 1;
    }
    numbers = expanded;
    for (size_t i = 5; i < 8; ++i)
    {
        numbers[i] = (int)(i * 2);
    }
    for (size_t i = 0; i < 8; ++i)
    {
        printf("numbers[%zu]=%d\n", i, numbers[i]);
    }
    free(numbers);
    numbers = NULL;
    return 0;
}
