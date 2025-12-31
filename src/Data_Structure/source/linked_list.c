#include "../include/linked_list.h"

linked_list *init_linked_list(void)
{
    linked_list *list = (linked_list *)malloc(sizeof(linked_list));
    if (list != NULL)
    {
        list->head = NULL;
        list->length = 0;
        return list;
    }
    else
    {
        fprintf(stderr, "Error: failed to allocate memory for linked list\n");
        return NULL;
    }
}

void append_node(int data, linked_list *list)
{
    if (list == NULL)
    {
        fprintf(stderr, "Error: linked list is NULL\n");
        return;
    }
    else
    {
        node *new_node = (node *)malloc(sizeof(node));
        if (new_node == NULL)
        {
            fprintf(stderr, "Error: failed to allocate memory for new node\n");
            return;
        }
        new_node->data = data;
        new_node->next = NULL;

        if (list->head == NULL)
        {
            list->head = new_node;
        }
        else
        {
            node *current = list->head;
            while (current->next != NULL)
            {
                current = current->next;
            }
            current->next = new_node;
        }
        list->length++;
    }
}

void insert_node(int data, linked_list *list, int index)
{
    // 1. 检查链表指针
    if (list == NULL)
    {
        fprintf(stderr, "Error: linked list is NULL\n");
        return;
    }

    // 2. 检查索引越界 (允许在末尾插入，即 index == list->length)
    if (index < 0 || index > list->length)
    {
        fprintf(stderr, "Error: index %d out of range [0, %d]\n", index, list->length);
        return;
    }

    // 3. 创建新节点
    node *new_node = (node *)malloc(sizeof(node));
    if (new_node == NULL)
    {
        fprintf(stderr, "Error: failed to allocate memory for new node\n");
        return;
    }
    new_node->data = data;
    new_node->next = NULL;

    // 4. 插入逻辑
    if (index == 0)
    {
        // 头部插入
        new_node->next = list->head;
        list->head = new_node;
    }
    else
    {
        // 中间或尾部插入
        // 寻找第 index-1 个节点
        node *prev = list->head;
        for (int i = 0; i < index - 1; i++)
        {
            // 防御性检查：如果 length 字段不准，防止空指针解引用
            if (prev == NULL)
            {
                fprintf(stderr, "Error: corrupted list structure (unexpected NULL)\n");
                free(new_node);
                return;
            }
            prev = prev->next;
        }

        // 执行插入操作
        new_node->next = prev->next;
        prev->next = new_node;
    }

    // 5. 更新链表长度
    list->length++;
}

void delete_node(linked_list *list, int index)
{
    if (list == NULL)
    {
        fprintf(stderr, "Error: linked list is NULL\n");
        return;
    }
    else if (index < 0 || index >= list->length)
    {
        fprintf(stderr, "Error: index %d out of range [0, %d)\n", index, list->length);
        return;
    }

    node *to_delete = NULL;

    /* 删除头节点 */
    if (index == 0)
    {
        to_delete = list->head;
        if (to_delete == NULL)
        {
            /* 空表，但按前面的检查不会到这里 */
            return;
        }
        list->head = to_delete->next;
        free(to_delete);
        list->length--;
        return;
    }

    /* 寻找要删除节点的前一个节点 (index-1) */
    node *prev = list->head;
    for (int i = 0; i < index - 1; i++)
    {
        if (prev == NULL)
        {
            fprintf(stderr, "Error: corrupted list structure (unexpected NULL)\n");
            return;
        }
        prev = prev->next;
    }

    if (prev == NULL || prev->next == NULL)
    {
        fprintf(stderr, "Error: cannot locate node to delete (index %d)\n", index);
        return;
    }

    to_delete = prev->next;
    prev->next = to_delete->next;
    free(to_delete);
    list->length--;
}

void destroy_linked_list(linked_list *list)
{
    if(list == NULL){
        return;
    }
    node *current = list->head;
    // node *next = current->next;
    while(current != NULL){
        node *next = current->next;
        free(current);
        current = next;
    }
    // current == NULL
    list->length = 0;
    list->head = NULL;
    free(list);
}

void show_linked_list(const linked_list *list)
{
    if (list == NULL)
    {
        fprintf(stderr, "Error: linked list is NULL\n");
        return;
    }

    node *current = list->head;
    printf("Linked list contents: ");
    while (current != NULL)
    {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}