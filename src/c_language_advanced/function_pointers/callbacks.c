#include <stdio.h>
#include <stdlib.h>

typedef int (*cmp_fn)(const void *, const void *);

/*
 * compare_int - comparator for integers used with qsort
 * @lhs, @rhs: pointers to elements (typed as const void* because that's
 *              the signature qsort expects). Internally we cast them to
 *              pointers to int and then dereference to obtain the values.
 *
 * Return value convention required by qsort:
 *   negative if *lhs < *rhs,
 *   zero     if *lhs == *rhs,
 *   positive if *lhs > *rhs.
 *
 * Implementation note:
 *   The expression '(a > b) - (a < b)' yields 1, 0 or -1 without branches:
 *     - when a > b:  (1) - (0) ->  1
 *     - when a == b: (0) - (0) ->  0
 *     - when a < b:  (0) - (1) -> -1
 *   This is a compact, well-defined way to produce the sign of (a - b)
 *   while avoiding overflow and avoiding an if/else branch.
 */
static int compare_int(const void *lhs, const void *rhs)
{
    /* Cast the void pointers to pointers to int and dereference */
    int a = *(const int *)lhs;
    int b = *(const int *)rhs;

    /* Return -1, 0, or 1 depending on the comparison */
    return (a > b) - (a < b);
}

static void print_array(const int *values, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        printf("%d ", values[i]);
    }
    putchar('\n');
}

int main(void)
{
    int data[] = {7, 3, 9, 1, 5};
    size_t count = sizeof data / sizeof data[0];
    puts("original:");
    print_array(data, count);
    qsort(data, count, sizeof(int), compare_int);
    puts("sorted:");
    print_array(data, count);
    return 0;
}
