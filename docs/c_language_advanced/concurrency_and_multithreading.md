# 并发与多线程

## 基础概念
- 并发：多个任务在时间上交织执行，可能共享资源。
- 多线程：同一进程内的多个执行流，共享地址空间但拥有独立栈。
- 为保证数据一致性，需要同步原语防止竞态条件。

## POSIX 线程 (pthread)
- 创建线程：`pthread_create`，传入线程函数和上下文。
- 结束线程：`pthread_join` 等待线程结束并回收资源。
- 分离线程：`pthread_detach`，线程结束后自动回收。

```c
#include <pthread.h>

typedef struct {
    int id;
} WorkerArgs;

void *worker(void *arg) {
    WorkerArgs *ctx = arg;
    printf("worker %d\n", ctx->id);
    return NULL;
}

int main(void) {
    pthread_t tid;
    WorkerArgs args = { .id = 1 };
    if (pthread_create(&tid, NULL, worker, &args) != 0) {
        perror("pthread_create");
        return 1;
    }
    pthread_join(tid, NULL);
    return 0;
}
```

## 同步原语
- 互斥锁 `pthread_mutex_t`：保护临界区，使用 `pthread_mutex_lock`/`unlock`。
- 读写锁 `pthread_rwlock_t`：读多写少场景更高效。
- 条件变量 `pthread_cond_t`：线程等待特定条件，多与互斥锁配合。
- 信号量 `sem_t`：计数器，用于资源池管理或限流。

## 原子操作
- C11 提供 `<stdatomic.h>` 原子类型与操作，如 `atomic_int`、`atomic_fetch_add`。
- GCC/Clang 还提供 `__sync_*`、`__atomic_*` 内建函数。
- 原子操作保证单条操作的原子性，但复杂逻辑仍需锁或序列一致性控制。

## 内存模型
- C11 定义内存序，常用：
  - `memory_order_relaxed`：仅保证原子性。
  - `memory_order_acquire/release`：建立有向同步关系。
  - `memory_order_seq_cst`：最强保证，易于理解但开销最大。
- 无锁编程需要仔细推导内存序，建议先使用锁保证正确性再优化。

## 竞态条件与死锁
- 竞态条件：多个线程同时访问共享数据且至少一个写入时未同步。
- 死锁：多个线程互相等待对方持有的资源。避免策略：统一加锁顺序、使用带超时的锁、尽量缩小锁粒度。

## 线程安全设计
- 尽量使用不可变数据结构，减少共享可变状态。
- 利用线程局部存储 `__thread` 或 `pthread_key_t`，给每个线程提供独立数据。
- 配合消息队列或任务队列采用生产者-消费者模型，降低锁竞争。

## 调试并发问题
- 工具：`helgrind`、`drd`(Valgrind 工具) 检测数据竞争；`ThreadSanitizer`(`-fsanitize=thread`) 自动发现竞态。
- 记录日志带线程 ID：`pthread_self()` 或跨平台库提供的 API。
- 在测试环境启用调度随机化（如 `TSAN_OPTIONS=halt_on_error=1`）增加问题复现概率。

## 其他并发模型
- 事件驱动：单线程事件循环结合非阻塞 I/O，如 `libevent`、`libuv`。
- 进程池：通过 `fork` 创建多个工作进程，使用 IPC 管理任务。
- 协程与用户态线程：如 `libco`、`boost::context`，通过切换堆栈实现轻量级并发。

## 总结
- 并发编程需先确保正确性，再考虑性能。
- 充分利用现有库和同步原语，避免自行实现复杂锁机制。
- 写单元测试和压力测试覆盖边界场景，结合工具检测竞态和死锁。
