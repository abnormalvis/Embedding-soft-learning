#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include "Data_Base.h"
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct doubly_node
{
    int data;
    struct doubly_node *prev;
    struct doubly_node *next;
} doubly_node;

typedef struct doubly_list
{
    doubly_node *head;
    doubly_node *tail;
    int length;
} doubly_list;

// 创建 / 销毁
doubly_list *dll_init(void);
void dll_destroy(doubly_list *list);

// 基本信息
bool dll_is_empty(const doubly_list *list);
int dll_length(const doubly_list *list);

// 插入（允许 index == length 表示尾插）
bool dll_insert_at(doubly_list *list, int index, int value);
bool dll_push_front(doubly_list *list, int value);
bool dll_push_back(doubly_list *list, int value);

// 删除（若 out_value != NULL 则返回被删除值）
bool dll_delete_at(doubly_list *list, int index, int *out_value);
bool dll_pop_front(doubly_list *list, int *out_value);
bool dll_pop_back(doubly_list *list, int *out_value);

// 访问
bool dll_get_at(const doubly_list *list, int index, int *out_value);
int dll_find_first(const doubly_list *list, int value);

// 展示
void dll_print_forward(const doubly_list *list);
void dll_print_backward(const doubly_list *list);

#ifdef __cplusplus
}
#endif

#endif // DOUBLY_LINKED_LIST_H