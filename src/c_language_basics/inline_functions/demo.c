#include <stdio.h>
static inline int clamp_int(int value, int min, int max) {
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}
static inline double lerp(double a, double b, double t) {
    return a + (b - a) * t;
}
int main(void) {
    int original = 42;
    int clamped = clamp_int(original, 0, 20);
    double blended = lerp(0.0, 10.0, 0.3);
    printf("clamped=%d blended=%.2f\n", clamped, blended);
    return 0;
}
