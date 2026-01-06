#include "../include/list_queue.h"

/**
 * 创建一个新的链式队列
 * @return 队列指针，失败返回 NULL
 */
queue *create_queue(void)
{
    queue *q = (queue *)malloc(sizeof(queue));
    if (q == NULL)
    {
        fprintf(stderr, "Error: failed to allocate memory for queue\n");
        return NULL;
    }
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
    return q;
}

/**
 * 检查队列是否为空
 * @param q 队列指针
 * @return 1 表示空，0 表示非空
 */
int is_empty(queue *q)
{
    return (q == NULL || q->size == 0);
}

/**
 * 入队操作（在队尾插入元素）
 * @param q 队列指针
 * @param data 要插入的数据
 */
void enqueue(queue *q, int data)
{
    if (q == NULL)
    {
        fprintf(stderr, "Error: queue is NULL\n");
        return;
    }

    // 创建新节点
    node *new_node = (node *)malloc(sizeof(node));
    if (new_node == NULL)
    {
        fprintf(stderr, "Error: failed to allocate memory for new node\n");
        return;
    }
    new_node->data = data;
    new_node->next = NULL;

    // 如果队列为空，front 和 rear 都指向新节点
    if (q->rear == NULL)
    {
        q->front = new_node;
        q->rear = new_node;
    }
    else
    {
        // 否则将新节点链接到 rear 后面，并更新 rear
        q->rear->next = new_node;
        q->rear = new_node;
    }
    q->size++;
}

/**
 * 出队操作（从队头删除元素）
 * @param q 队列指针
 * @return 队头元素的值，队列为空返回 -1
 */
int dequeue(queue *q)
{
    if (q == NULL)
    {
        fprintf(stderr, "Error: queue is NULL\n");
        return -1;
    }

    if (is_empty(q))
    {
        fprintf(stderr, "Queue underflow\n");
        return -1;
    }

    node *temp = q->front;
    int data = temp->data;

    // 更新 front 指针
    q->front = q->front->next;

    // 如果队列变空，rear 也要置为 NULL
    if (q->front == NULL)
    {
        q->rear = NULL;
    }

    free(temp);
    q->size--;
    return data;
}

/**
 * 查看队头元素但不删除
 * @param q 队列指针
 * @return 队头元素的值，队列为空返回 -1
 */
int peek(queue *q)
{
    if (q == NULL)
    {
        fprintf(stderr, "Error: queue is NULL\n");
        return -1;
    }

    if (is_empty(q))
    {
        fprintf(stderr, "Queue is empty\n");
        return -1;
    }

    return q->front->data;
}

/**
 * 销毁队列，释放所有内存
 * @param q 队列指针
 */
void destroy_queue(queue *q)
{
    if (q == NULL)
    {
        return;
    }

    // 释放所有节点
    node *current = q->front;
    while (current != NULL)
    {
        node *next = current->next;
        free(current);
        current = next;
    }

    // 释放队列结构本身
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
    free(q);
}