#include <stdio.h>
static unsigned pack_rgb(unsigned r, unsigned g, unsigned b)
{
    return ((r & 0xFFu) << 16) | ((g & 0xFFu) << 8) | (b & 0xFFu);
}
int main(void)
{
    int a = 5;
    int b = 3;
    int arithmetic = a * a + b;
    int comparison = (a > b) && (b > 0);
    unsigned mask = 0u;
    mask |= 1u << 1;
    mask ^= 1u << 3;
    printf("arithmetic=%d comparison=%d mask=0x%X\n", arithmetic, comparison, mask);
    unsigned color = pack_rgb(0x12, 0x34, 0x56);
    printf("color=0x%06X\n", color);
    return 0;
}
