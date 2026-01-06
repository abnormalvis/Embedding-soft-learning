#include "include/list_queue.h"
#include <assert.h>
#include <stdio.h>

void test_create_and_destroy()
{
    printf("\n=== Test: Create and Destroy ===\n");
    
    queue *q = create_queue();
    assert(q != NULL);
    assert(is_empty(q));
    assert(q->size == 0);
    printf("✓ Created empty queue\n");
    
    destroy_queue(q);
    printf("✓ Queue destroyed successfully\n");
}

void test_basic_enqueue_dequeue()
{
    printf("\n=== Test: Basic Enqueue/Dequeue ===\n");
    
    queue *q = create_queue();
    
    // 入队
    enqueue(q, 10);
    enqueue(q, 20);
    enqueue(q, 30);
    assert(q->size == 3);
    assert(!is_empty(q));
    printf("✓ Enqueued 3 elements\n");
    
    // 查看队头
    int front = peek(q);
    assert(front == 10);
    printf("✓ Front element is 10\n");
    
    // 出队
    int val = dequeue(q);
    assert(val == 10);
    assert(q->size == 2);
    printf("✓ Dequeued 10\n");
    
    val = dequeue(q);
    assert(val == 20);
    assert(q->size == 1);
    printf("✓ Dequeued 20\n");
    
    val = dequeue(q);
    assert(val == 30);
    assert(q->size == 0);
    assert(is_empty(q));
    printf("✓ Dequeued 30, queue is now empty\n");
    
    destroy_queue(q);
}

void test_fifo_order()
{
    printf("\n=== Test: FIFO Order ===\n");
    
    queue *q = create_queue();
    
    // 入队 1-5
    for (int i = 1; i <= 5; i++)
    {
        enqueue(q, i);
    }
    
    printf("✓ Enqueued: 1, 2, 3, 4, 5\n");
    
    // 验证 FIFO 顺序
    for (int i = 1; i <= 5; i++)
    {
        int val = dequeue(q);
        assert(val == i);
    }
    
    printf("✓ Dequeued in FIFO order: 1, 2, 3, 4, 5\n");
    assert(is_empty(q));
    
    destroy_queue(q);
}

void test_boundary_conditions()
{
    printf("\n=== Test: Boundary Conditions ===\n");
    
    queue *q = create_queue();
    
    // 空队列出队
    int val = dequeue(q);
    assert(val == -1);
    printf("✓ Dequeue from empty queue returns -1\n");
    
    // 空队列 peek
    val = peek(q);
    assert(val == -1);
    printf("✓ Peek empty queue returns -1\n");
    
    // 单元素操作
    enqueue(q, 42);
    assert(peek(q) == 42);
    assert(dequeue(q) == 42);
    assert(is_empty(q));
    printf("✓ Single element enqueue/dequeue works\n");
    
    destroy_queue(q);
}

void test_enqueue_after_dequeue()
{
    printf("\n=== Test: Enqueue After Dequeue ===\n");
    
    queue *q = create_queue();
    
    // 先入队一些元素
    enqueue(q, 1);
    enqueue(q, 2);
    enqueue(q, 3);
    
    // 出队一些元素
    dequeue(q);
    dequeue(q);
    
    // 再入队
    enqueue(q, 4);
    enqueue(q, 5);
    
    // 验证顺序
    assert(dequeue(q) == 3);
    assert(dequeue(q) == 4);
    assert(dequeue(q) == 5);
    assert(is_empty(q));
    
    printf("✓ Mixed enqueue/dequeue operations work correctly\n");
    
    destroy_queue(q);
}

void test_large_queue()
{
    printf("\n=== Test: Large Queue ===\n");
    
    queue *q = create_queue();
    
    // 入队大量元素
    const int N = 1000;
    for (int i = 0; i < N; i++)
    {
        enqueue(q, i);
    }
    assert(q->size == N);
    printf("✓ Enqueued %d elements\n", N);
    
    // 出队并验证
    for (int i = 0; i < N; i++)
    {
        int val = dequeue(q);
        assert(val == i);
    }
    assert(is_empty(q));
    printf("✓ Dequeued %d elements in correct order\n", N);
    
    destroy_queue(q);
}

int main(void)
{
    printf("========================================\n");
    printf("  Linked Queue Implementation Test\n");
    printf("========================================\n");
    
    test_create_and_destroy();
    test_basic_enqueue_dequeue();
    test_fifo_order();
    test_boundary_conditions();
    test_enqueue_after_dequeue();
    test_large_queue();
    
    printf("\n========================================\n");
    printf("  All Queue Tests Passed! ✓\n");
    printf("========================================\n");
    
    return 0;
}
