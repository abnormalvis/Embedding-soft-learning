#include "counter.h"
#include <stdio.h>
int main(void) {
    Counter *counter = counter_create();
    if (!counter) {
        fputs("failed to allocate counter\n", stderr);
        return 1;
    }
    for (int i = 0; i < 3; ++i) {
        counter_increment(counter);
    }
    printf("counter=%zu\n", counter_value(counter));
    counter_destroy(counter);
    return 0;
}
