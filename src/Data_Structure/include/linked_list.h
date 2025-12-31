#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "Data_Base.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>

typedef struct node {
    int data;
    struct node *next;
} node;

typedef struct linked_list {
    node *head;
    int length;
} linked_list;

linked_list* init_linked_list(void);
void append_node(int data, linked_list *list);
void insert_node(int data, linked_list *list, int index);
void delete_node(linked_list *list, int index);
void destroy_linked_list(linked_list *list);
void show_linked_list(const linked_list *list);

#endif // LINKED_LIST_H
