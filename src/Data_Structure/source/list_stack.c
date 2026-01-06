#include "../include/list_stack.h"

stack *create_stack()
{
    stack *s = (stack *)malloc(sizeof(stack));
    if (s)
    {
        s->top = NULL;
        s->size = 0;
    }
    return s;
}

int is_empty(stack *s)
{
    return s->size == 0;
}

void push(stack *s, int data)
{
    node *new_node = (node *)malloc(sizeof(node));
    if (!new_node)
        return;

    new_node->data = data;
    new_node->next = s->top;
    s->top = new_node;
    s->size++;
}

int pop(stack *s)
{
    if (is_empty(s))
    {
        fprintf(stderr, "Stack underflow\n");
        return -1;
    }

    node *temp = s->top;
    int popped_data = temp->data;
    s->top = s->top->next;
    free(temp);
    s->size--;
    return popped_data;
}

int peek(stack *s)
{
    if (is_empty(s))
        return -1;

    return s->top->data;
}
void destroy_stack(stack *s)
{
    while (!is_empty(s))
    {
        pop(s);
    }
    free(s);
}