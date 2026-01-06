#ifndef LIST_QUEUE_H
#define LIST_QUEUE_H

#include "Data_Base.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>

typedef struct node
{
    int data;
    struct node *next;
} node;

typedef struct
{
    node *front;
    node *rear;
    int size;
} queue;

queue *create_queue(void);
int is_empty(queue *q);
void enqueue(queue *q, int data);
int dequeue(queue *q);
int peek(queue *q);
void destroy_queue(queue *q);
#endif // LIST_QUEUE_H
