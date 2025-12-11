#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
struct Node {
    uint32_t id;
    double value;
};
int main(void) {
    struct Node nodes[3] = {
        { 1u, 1.5 },
        { 2u, 3.0 },
        { 3u, 4.5 }
    };
    struct Node *begin = nodes;
    struct Node *end = nodes + 3;
    double total = 0.0;
    for (struct Node *p = begin; p < end; ++p) {
        total += p->value;
    }
    printf("total=%.1f\n", total);
    printf("sizeof(Node)=%zu offset(value)=%zu\n", sizeof(struct Node), offsetof(struct Node, value));
    void *raw = begin;
    printf("raw pointer=%p first id=%u\n", raw, begin->id);
    return 0;
}
