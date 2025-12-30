#include "../include/sequential_list.h"

/**
 * 创建一个新的顺序表
 * @param capacity 初始容量
 * @return 指向新顺序表的指针，失败返回 NULL
 */
sqlist *create_sequential_list(int capacity)
{
    if (capacity <= 0)
    {
        fprintf(stderr, "Error: capacity must be positive\n");
        return NULL;
    }

    sqlist *list = (sqlist *)malloc(sizeof(sqlist));
    if (list == NULL)
    {
        fprintf(stderr, "Error: failed to allocate memory for list\n");
        return NULL;
    }

    list->arr = (int *)malloc(sizeof(int) * capacity);
    if (list->arr == NULL)
    {
        fprintf(stderr, "Error: failed to allocate memory for array\n");
        free(list);
        return NULL;
    }

    list->length = 0;
    list->capacity = capacity;
    return list;
}

/**
 * 销毁顺序表，释放内存
 */
void destroy_sequential_list(sqlist *list)
{
    if (list != NULL)
    {
        if (list->arr != NULL)
        {
            free(list->arr);
        }
        free(list);
    }
}

/**
 * 检查顺序表是否为空
 */
bool is_empty(sqlist *list)
{
    return (list == NULL || list->length == 0);
}

/**
 * 检查顺序表是否已满
 */
bool is_full(sqlist *list)
{
    return (list != NULL && list->length >= list->capacity);
}

/**
 * 获取顺序表当前长度
 */
int get_length(sqlist *list)
{
    return (list != NULL) ? list->length : 0;
}

/**
 * 清空顺序表
 */
void clear_list(sqlist *list)
{
    if (list != NULL)
    {
        list->length = 0;
    }
}

/**
 * 在指定位置插入元素
 * @param list 顺序表指针
 * @param index 插入位置 (0 到 length)
 * @param value 要插入的值
 * @return 成功返回 true，失败返回 false
 */
bool insert_at(sqlist *list, int index, int value)
{
    if (list == NULL)
    {
        fprintf(stderr, "Error: list is NULL\n");
        return false;
    }

    if (index < 0 || index > list->length)
    {
        fprintf(stderr, "Error: index out of range [0, %d]\n", list->length);
        return false;
    }

    if (is_full(list))
    {
        fprintf(stderr, "Error: list is full\n");
        return false;
    }

    // 将 index 及其后的元素后移
    for (int i = list->length; i > index; i--)
    {
        list->arr[i] = list->arr[i - 1];
    }

    list->arr[index] = value;
    list->length++;
    return true;
}

/**
 * 删除指定位置的元素
 * @param list 顺序表指针
 * @param index 删除位置 (0 到 length-1)
 * @param value 用于存储被删除的值（可为 NULL）
 * @return 成功返回 true，失败返回 false
 */
bool delete_at(sqlist *list, int index, int *value)
{
    if (list == NULL)
    {
        fprintf(stderr, "Error: list is NULL\n");
        return false;
    }

    if (is_empty(list))
    {
        fprintf(stderr, "Error: list is empty\n");
        return false;
    }

    if (index < 0 || index >= list->length)
    {
        fprintf(stderr, "Error: index out of range [0, %d)\n", list->length);
        return false;
    }

    // 保存被删除的值
    if (value != NULL)
    {
        *value = list->arr[index];
    }

    // 将 index 后的元素前移
    for (int i = index; i < list->length - 1; i++)
    {
        list->arr[i] = list->arr[i + 1];
    }

    list->length--;
    return true;
}

/**
 * 在表尾追加元素
 */
bool append(sqlist *list, int value)
{
    return insert_at(list, list->length, value);
}

/**
 * 查找元素首次出现的位置
 * @param list 顺序表指针
 * @param value 要查找的值
 * @return 元素的索引，未找到返回 -1
 */
int find_value(sqlist *list, int value)
{
    if (list == NULL || is_empty(list))
    {
        return -1;
    }

    for (int i = 0; i < list->length; i++)
    {
        if (list->arr[i] == value)
        {
            return i;
        }
    }

    return -1;
}

/**
 * 获取指定位置的元素
 * @param list 顺序表指针
 * @param index 位置索引
 * @param value 用于存储获取的值
 * @return 成功返回 true，失败返回 false
 */
bool get_at(sqlist *list, int index, int *value)
{
    if (list == NULL || value == NULL)
    {
        fprintf(stderr, "Error: NULL pointer\n");
        return false;
    }

    if (index < 0 || index >= list->length)
    {
        fprintf(stderr, "Error: index out of range [0, %d)\n", list->length);
        return false;
    }

    *value = list->arr[index];
    return true;
}

/**
 * 设置指定位置的元素
 * @param list 顺序表指针
 * @param index 位置索引
 * @param value 新的值
 * @return 成功返回 true，失败返回 false
 */
bool set_at(sqlist *list, int index, int value)
{
    if (list == NULL)
    {
        fprintf(stderr, "Error: list is NULL\n");
        return false;
    }

    if (index < 0 || index >= list->length)
    {
        fprintf(stderr, "Error: index out of range [0, %d)\n", list->length);
        return false;
    }

    list->arr[index] = value;
    return true;
}

/**
 * 打印顺序表内容
 */
void print_list(sqlist *list)
{
    if (list == NULL)
    {
        printf("List is NULL\n");
        return;
    }

    if (is_empty(list))
    {
        printf("List is empty\n");
        return;
    }

    printf("List (length=%d, capacity=%d): [", list->length, list->capacity);
    for (int i = 0; i < list->length; i++)
    {
        printf("%d", list->arr[i]);
        if (i < list->length - 1)
        {
            printf(", ");
        }
    }
    printf("]\n");
}
