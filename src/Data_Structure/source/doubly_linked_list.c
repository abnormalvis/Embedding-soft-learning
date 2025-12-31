
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

static doubly_node *dll_nth_node(const doubly_list *list, int index)
{
    // 调用方保证 index 合法: 0..length-1
    // 使用哨兵节点：从头或尾选择更近方向遍历
    if (index <= list->length / 2)
    {
        doubly_node *cur = list->sentinel->next;
        for (int i = 0; i < index; i++)
        {
            cur = cur->next;
        }
        return cur;
    }
    else
    {
        doubly_node *cur = list->sentinel->prev;
        for (int i = list->length - 1; i > index; i--)
        {
            cur = cur->prev;
        }
        return cur;
    }
}

static void dll_link_between(doubly_node *prev, doubly_node *next, doubly_node *node)
{
    node->prev = prev;
    node->next = next;
    prev->next = node;
    next->prev = node;
}

static void dll_unlink_node(doubly_node *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->prev = NULL;
    node->next = NULL;
}

doubly_list *dll_init(void)
{
    doubly_list *list = (doubly_list *)malloc(sizeof(doubly_list));
    if (list == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for doubly linked list\n");
        return NULL;
    }

    doubly_node *sentinel = dll_new_node(0);
    if (sentinel == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for sentinel node\n");
        free(list);
        return NULL;
    }

    // 循环哨兵：空表 next/prev 都指向自己
    sentinel->next = sentinel;
    sentinel->prev = sentinel;

    list->sentinel = sentinel;
    list->length = 0;
    return list;
}

void dll_destroy(doubly_list *list)
{
    if (list == NULL)
    {
        return;
    }

    if (list->sentinel != NULL)
    {
        doubly_node *cur = list->sentinel->next;
        while (cur != list->sentinel)
        {
            doubly_node *next = cur->next;
            free(cur);
            cur = next;
        }
        free(list->sentinel);
        list->sentinel = NULL;
    }

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

bool dll_insert_at(doubly_list *list, int index, int value)
{
    if (list == NULL || list->sentinel == NULL)
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

    // 插入位置：在“第 index 个节点”前插入
    // index == length => 在哨兵前插入（尾插）
    doubly_node *next = (index == list->length) ? list->sentinel : dll_nth_node(list, index);
    doubly_node *prev = next->prev;
    dll_link_between(prev, next, n);
    list->length++;
    return true;
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

bool dll_delete_at(doubly_list *list, int index, int *out_value)
{
    if (list == NULL || list->sentinel == NULL)
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

    doubly_node *node = dll_nth_node(list, index);
    if (out_value != NULL)
    {
        *out_value = node->data;
    }
    dll_unlink_node(node);
    free(node);
    list->length--;
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

bool dll_get_at(const doubly_list *list, int index, int *out_value)
{
    if (list == NULL || list->sentinel == NULL || out_value == NULL)
    {
        fprintf(stderr, "Error: NULL pointer\n");
        return false;
    }
    if (index < 0 || index >= list->length)
    {
        fprintf(stderr, "Error: index %d out of range [0, %d)\n", index, list->length);
        return false;
    }

    doubly_node *node = dll_nth_node(list, index);
    *out_value = node->data;
    return true;
}

int dll_find_first(const doubly_list *list, int value)
{
    if (list == NULL || list->sentinel == NULL)
    {
        return -1;
    }
    int i = 0;
    for (doubly_node *cur = list->sentinel->next; cur != list->sentinel; cur = cur->next)
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
    if (list == NULL || list->sentinel == NULL)
    {
        printf("DLL: (null)\n");
        return;
    }
    printf("DLL forward (len=%d): [", list->length);
    for (doubly_node *cur = list->sentinel->next; cur != list->sentinel; cur = cur->next)
    {
        printf("%d", cur->data);
        if (cur->next != list->sentinel)
        {
            printf(", ");
        }
    }
    printf("]\n");
}

void dll_print_backward(const doubly_list *list)
{
    if (list == NULL || list->sentinel == NULL)
    {
        printf("DLL: (null)\n");
        return;
    }
    printf("DLL backward (len=%d): [", list->length);
    for (doubly_node *cur = list->sentinel->prev; cur != list->sentinel; cur = cur->prev)
    {
        printf("%d", cur->data);
        if (cur->prev != list->sentinel)
        {
            printf(", ");
        }
    }
    printf("]\n");
}
