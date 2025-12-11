#ifndef MODULAR_DESIGN_COUNTER_H
#define MODULAR_DESIGN_COUNTER_H
#include <stddef.h>
typedef struct Counter Counter;
Counter *counter_create(void);
void counter_destroy(Counter *counter);
void counter_increment(Counter *counter);
size_t counter_value(const Counter *counter);
#endif
