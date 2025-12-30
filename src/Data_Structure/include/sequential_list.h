#ifndef SEQUENTIAL_LIST_H
#define SEQUENTIAL_LIST_H

#include "Data_Base.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct sequential_list
{
    int *arr;           // 动态数组
    int length;         // 当前元素个数
    int capacity;       // 数组容量
} sqlist;

// 创建和销毁
sqlist *create_sequential_list(int capacity);
void destroy_sequential_list(sqlist *list);

// 基本操作
bool is_empty(sqlist *list);
bool is_full(sqlist *list);
int get_length(sqlist *list);
void clear_list(sqlist *list);

// 插入和删除
bool insert_at(sqlist *list, int index, int value);
bool delete_at(sqlist *list, int index, int *value);
bool append(sqlist *list, int value);

// 查找和访问
int find_value(sqlist *list, int value);
bool get_at(sqlist *list, int index, int *value);
bool set_at(sqlist *list, int index, int value);

// 显示
void print_list(sqlist *list);

#endif // SEQUENTIAL_LIST_H