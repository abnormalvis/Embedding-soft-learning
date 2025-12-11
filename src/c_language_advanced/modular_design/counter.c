#include "counter.h"
#include <stdlib.h>
struct Counter {
    size_t value;
};
Counter *counter_create(void) {
    Counter *counter = malloc(sizeof *counter);
    if (!counter) {
        return NULL;
    }
    counter->value = 0;
    return counter;
}
void counter_destroy(Counter *counter) {
    free(counter);
}
void counter_increment(Counter *counter) {
    if (counter) {
        ++counter->value;
    }
}
size_t counter_value(const Counter *counter) {
    return counter ? counter->value : 0;
}
