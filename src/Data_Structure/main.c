#include <stdio.h>
#include <stdlib.h>
#include "list_stack.h"

int main(void)
{
    /******** Stack Operations (Demo) ********/
    // create a stack
    stack *s = create_stack();
    if (!s)
    {
        fprintf(stderr, "Failed to create stack\n");
        return 1;
    }

    // use the stack
    push(s, 10);
    push(s, 20);
    push(s, 30);

    printf("Top element is %d\n", peek(s));

    printf("Popped element is %d\n", pop(s));
    printf("Popped element is %d\n", pop(s));
    printf("Popped element is %d\n", pop(s));
    printf("Popped element is %d\n", pop(s));
    destroy_stack(s);
    return 0;
}