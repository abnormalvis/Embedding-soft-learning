#include <stdio.h>
#define SCALE(value) ((value) * 2)
static int helper(int x)
{
    return x * x;
}
int main(void)
{
    int raw = 7;
    int scaled = SCALE(raw);
    int squared = helper(raw);
    printf("raw=%d scaled=%d squared=%d\n", raw, scaled, squared);
    return 0;
}
