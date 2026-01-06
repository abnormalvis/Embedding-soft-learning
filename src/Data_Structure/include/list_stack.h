#ifndef LIST_STACK_H
#define LIST_STACK_H

#include "Data_Base.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>

typedef struct
{
    int data;
    struct node *next;
} node;

typedef struct
{
    node *top;
    int size;
} stack;

stack *create_stack();
int is_empty(stack *s);
void push(stack *s, int data);
int pop(stack *s);
int peek(stack *s);
void destroy_stack(stack *s);

#endif // LIST_STACK_H
