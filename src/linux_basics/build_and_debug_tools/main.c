#include <stdio.h>
int helper(int value) {
    return value * 2;
}
int main(void) {
    for (int i = 0; i < 3; ++i) {
        printf("helper(%d)=%d\n", i, helper(i));
    }
    return 0;
}
