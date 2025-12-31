#include "../include/doubly_linked_list.h"

#include <stdlib.h>

static doubly_node *dll_new_node(int value)
{
    doubly_node *n = (doubly_node *)malloc(sizeof(doubly_node));
    if (n == NULL)
    {
        return NULL;
    }
    n->data = value;
    n->prev = NULL;
    n->next = NULL;
    return n;
}

doubly_list *dll_init(void)
{
    doubly_list *list = (doubly_list *)malloc(sizeof(doubly_list));
    if (list == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for doubly linked list\n");
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    return list;
}

void dll_destroy(doubly_list *list)
{
    if (list == NULL)
    {
        return;
    }
    doubly_node *cur = list->head;
    while (cur != NULL)
    {
        doubly_node *next = cur->next;
        free(cur);
        cur = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    free(list);
}

bool dll_is_empty(const doubly_list *list)
{
    return (list == NULL || list->length == 0);
}

int dll_length(const doubly_list *list)
{
    return (list != NULL) ? list->length : 0;
}

bool dll_push_front(doubly_list *list, int value)
{
    return dll_insert_at(list, 0, value);
}

bool dll_push_back(doubly_list *list, int value)
{
    if (list == NULL)
    {
        fprintf(stderr, "Error: list is NULL\n");
        return false;
    }
    return dll_insert_at(list, list->length, value);
}

bool dll_insert_at(doubly_list *list, int index, int value)
{
    if (list == NULL)
    {
        fprintf(stderr, "Error: list is NULL\n");
        return false;
    }
    if (index < 0 || index > list->length)
    {
        fprintf(stderr, "Error: index %d out of range [0, %d]\n", index, list->length);
        return false;
    }

    doubly_node *n = dll_new_node(value);
    if (n == NULL)
    {
        fprintf(stderr, "Error: failed to allocate memory for new node\n");
        return false;
    }

    // 空表
    if (list->length == 0)
    {
        list->head = n;
        list->tail = n;
        list->length = 1;
        return true;
    }

    // 头插
    if (index == 0)
    {
        n->next = list->head;
        list->head->prev = n;
        list->head = n;
        list->length++;
        return true;
    }

    // 尾插
    if (index == list->length)
    {
        n->prev = list->tail;
        list->tail->next = n;
        list->tail = n;
        list->length++;
        return true;
    }

    // 中间插入：找到 index 位置的节点（插入在它前面）
    doubly_node *cur = NULL;
    if (index <= list->length / 2)
    {
        cur = list->head;
        for (int i = 0; i < index; i++)
        {
            cur = cur->next;
        }
    }
    else
    {
        cur = list->tail;
        for (int i = list->length; i > index; i--)
        {
            cur = cur->prev;
        }
    }

    if (cur == NULL || cur->prev == NULL)
    {
        fprintf(stderr, "Error: corrupted list structure (unexpected NULL)\n");
        free(n);
        return false;
    }

    n->prev = cur->prev;
    n->next = cur;
    cur->prev->next = n;
    cur->prev = n;
    list->length++;
    return true;
}

bool dll_pop_front(doubly_list *list, int *out_value)
{
    return dll_delete_at(list, 0, out_value);
}

bool dll_pop_back(doubly_list *list, int *out_value)
{
    if (list == NULL)
    {
        fprintf(stderr, "Error: list is NULL\n");
        return false;
    }
    return dll_delete_at(list, list->length - 1, out_value);
}

bool dll_delete_at(doubly_list *list, int index, int *out_value)
{
    if (list == NULL)
    {
        fprintf(stderr, "Error: list is NULL\n");
        return false;
    }
    if (list->length == 0)
    {
        fprintf(stderr, "Error: list is empty\n");
        return false;
    }
    if (index < 0 || index >= list->length)
    {
        fprintf(stderr, "Error: index %d out of range [0, %d)\n", index, list->length);
        return false;
    }

    // 单节点
    if (list->length == 1)
    {
        if (out_value != NULL)
        {
            *out_value = list->head->data;
        }
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
        list->length = 0;
        return true;
    }

    // 删除头
    if (index == 0)
    {
        doubly_node *old = list->head;
        if (out_value != NULL)
        {
            *out_value = old->data;
        }
        list->head = old->next;
        list->head->prev = NULL;
        free(old);
        list->length--;
        return true;
    }

    // 删除尾
    if (index == list->length - 1)
    {
        doubly_node *old = list->tail;
        if (out_value != NULL)
        {
            *out_value = old->data;
        }
        list->tail = old->prev;
        list->tail->next = NULL;
        free(old);
        list->length--;
        return true;
    }

    // 删除中间
    doubly_node *cur = NULL;
    if (index <= list->length / 2)
    {
        cur = list->head;
        for (int i = 0; i < index; i++)
        {
            cur = cur->next;
        }
    }
    else
    {
        cur = list->tail;
        for (int i = list->length - 1; i > index; i--)
        {
            cur = cur->prev;
        }
    }

    if (cur == NULL || cur->prev == NULL || cur->next == NULL)
    {
        fprintf(stderr, "Error: corrupted list structure (unexpected NULL)\n");
        return false;
    }

    if (out_value != NULL)
    {
        *out_value = cur->data;
    }
    cur->prev->next = cur->next;
    cur->next->prev = cur->prev;
    free(cur);
    list->length--;
    return true;
}

bool dll_get_at(const doubly_list *list, int index, int *out_value)
{
    if (list == NULL || out_value == NULL)
    {
        fprintf(stderr, "Error: NULL pointer\n");
        return false;
    }
    if (index < 0 || index >= list->length)
    {
        fprintf(stderr, "Error: index %d out of range [0, %d)\n", index, list->length);
        return false;
    }

    doubly_node *cur = NULL;
    if (index <= list->length / 2)
    {
        cur = list->head;
        for (int i = 0; i < index; i++)
        {
            cur = cur->next;
        }
    }
    else
    {
        cur = list->tail;
        for (int i = list->length - 1; i > index; i--)
        {
            cur = cur->prev;
        }
    }

    *out_value = cur->data;
    return true;
}

int dll_find_first(const doubly_list *list, int value)
{
    if (list == NULL)
    {
        return -1;
    }
    int i = 0;
    for (doubly_node *cur = list->head; cur != NULL; cur = cur->next)
    {
        if (cur->data == value)
        {
            return i;
        }
        i++;
    }
    return -1;
}

void dll_print_forward(const doubly_list *list)
{
    if (list == NULL)
    {
        printf("DLL: (null)\n");
        return;
    }
    printf("DLL forward (len=%d): [", list->length);
    for (doubly_node *cur = list->head; cur != NULL; cur = cur->next)
    {
        printf("%d", cur->data);
        if (cur->next != NULL)
        {
            printf(", ");
        }
    }
    printf("]\n");
}

void dll_print_backward(const doubly_list *list)
{
    if (list == NULL)
    {
        printf("DLL: (null)\n");
        return;
    }
    printf("DLL backward (len=%d): [", list->length);
    for (doubly_node *cur = list->tail; cur != NULL; cur = cur->prev)
    {
        printf("%d", cur->data);
        if (cur->prev != NULL)
        {
            printf(", ");
        }
    }
    printf("]\n");
}
