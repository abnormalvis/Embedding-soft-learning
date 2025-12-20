# 并发与多线程

## 目录
- [基础概念](#基础概念)
- [进程 vs 线程](#进程-vs-线程)
- [POSIX 线程 (pthread)](#posix-线程-pthread)
- [同步原语](#同步原语)
- [原子操作](#原子操作)
- [内存模型](#内存模型)
- [竞态条件与死锁](#竞态条件与死锁)
- [线程安全设计](#线程安全设计)
- [线程池实现](#线程池实现)
- [生产者-消费者模型](#生产者-消费者模型)
- [调试并发问题](#调试并发问题)
- [性能优化技巧](#性能优化技巧)
- [其他并发模型](#其他并发模型)

## 基础概念

### 什么是并发（Concurrency）？

**并发**是指多个任务在时间上交织执行的能力。这些任务可能：
- 在单核 CPU 上通过时间片轮转交替执行（并发但不并行）
- 在多核 CPU 上真正同时执行（并发且并行）

```
单核 CPU 上的并发（时间片轮转）：
时间 ─────────────────────────────────>
CPU  [任务A][任务B][任务A][任务C][任务B][任务A]
     虽然同一时刻只执行一个任务，但从宏观看多个任务在"同时"进行

多核 CPU 上的并行：
时间 ─────────────────────────────────>
核1  [任务A][任务A][任务A][任务A][任务A]
核2  [任务B][任务B][任务B][任务B][任务B]
核3  [任务C][任务C][任务C][任务C][任务C]
     多个任务真正同时执行
```

**关键术语对比：**

| 术语 | 定义 | 特点 | 例子 |
|------|------|------|------|
| **并发<br>(Concurrency)** | 多个任务在时间段内交替执行 | 逻辑上同时进行 | 单核 CPU 上运行多个程序 |
| **并行<br>(Parallelism)** | 多个任务在同一时刻执行 | 物理上同时进行 | 多核 CPU 同时执行多个线程 |
| **异步<br>(Asynchronous)** | 任务开始后不等待完成 | 非阻塞 | I/O 操作使用回调 |
| **同步<br>(Synchronous)** | 任务开始后等待完成 | 阻塞 | 普通函数调用 |

### 什么是多线程（Multithreading）？

**线程**是操作系统调度的最小单位，是进程中的一个执行流。同一进程内的多个线程：
- ✅ **共享**：代码段、数据段、堆内存、文件描述符、信号处理器
- ❌ **独立**：栈空间、寄存器、线程 ID、信号掩码、errno

```
进程内存布局（多线程视角）：

高地址
┌────────────────────┐
│   内核空间         │  所有线程共享
├────────────────────┤
│   线程1的栈        │  ← 独立（约 8MB）
├────────────────────┤
│   线程2的栈        │  ← 独立（约 8MB）
├────────────────────┤
│   线程3的栈        │  ← 独立（约 8MB）
├────────────────────┤
│   主线程的栈       │  ← 独立
├────────────────────┤
│                    │
│   堆（Heap）       │  ← 共享！需要同步
│                    │
├────────────────────┤
│ BSS 段（未初始化） │  ← 共享
├────────────────────┤
│ 数据段（全局变量） │  ← 共享！需要同步
├────────────────────┤
│ 代码段（只读）     │  ← 共享（安全）
└────────────────────┘
低地址

关键点：
1. 每个线程有独立的栈 → 局部变量是线程安全的
2. 全局变量和堆内存是共享的 → 需要同步机制保护
3. 代码段是只读的 → 多个线程执行同一函数是安全的
```

### 为什么需要并发？

1. **提高响应性**：GUI 程序用单独线程处理 UI，避免界面卡顿
2. **提高吞吐量**：利用多核 CPU 并行处理，加快计算速度
3. **简化程序结构**：将复杂任务分解为多个独立的并发任务
4. **资源利用**：在等待 I/O 时执行其他任务，而不是空闲等待

```c
// 示例：单线程 vs 多线程处理多个文件
// 单线程：顺序处理，总时间 = 所有文件处理时间之和
void process_files_single_thread(char **files, int count) {
    for (int i = 0; i < count; i++) {
        process_file(files[i]);  // 处理一个文件可能需要 1 秒
    }
    // 10 个文件需要 10 秒
}

// 多线程：并行处理，总时间 ≈ 最慢文件的处理时间
void process_files_multi_thread(char **files, int count) {
    pthread_t threads[count];
    for (int i = 0; i < count; i++) {
        pthread_create(&threads[i], NULL, process_file, files[i]);
    }
    for (int i = 0; i < count; i++) {
        pthread_join(threads[i], NULL);
    }
    // 在 10 核 CPU 上，10 个文件只需要约 1 秒
}
```

### 并发编程的挑战

1. **竞态条件（Race Condition）**：多个线程同时访问共享数据，结果取决于执行顺序
2. **死锁（Deadlock）**：多个线程互相等待对方释放资源
3. **活锁（Livelock）**：线程不断改变状态但无法继续执行
4. **饥饿（Starvation）**：某些线程永远无法获得所需资源
5. **优先级反转**：低优先级线程持有高优先级线程需要的资源
6. **调试困难**：问题难以重现，因为取决于线程调度的时序

## 进程 vs 线程

### 详细对比

| 特性 | 进程（Process） | 线程（Thread） |
|------|----------------|---------------|
| **定义** | 独立的执行实体 | 进程内的执行流 |
| **地址空间** | 独立的虚拟地址空间 | 共享进程的地址空间 |
| **内存隔离** | 完全隔离，安全 | 共享内存，需要同步 |
| **创建开销** | 大（需要复制地址空间） | 小（只需创建栈和寄存器） |
| **切换开销** | 大（需要切换地址空间） | 小（只需切换寄存器） |
| **通信方式** | IPC（管道、消息队列、共享内存） | 直接读写共享内存 |
| **通信开销** | 大（需要系统调用） | 小（直接内存访问） |
| **稳定性** | 一个进程崩溃不影响其他进程 | 一个线程崩溃可能导致整个进程崩溃 |
| **资源占用** | 每个进程独立占用资源 | 同一进程的线程共享资源 |
| **调试** | 相对容易（独立） | 较难（竞态条件） |
| **适用场景** | 完全独立的任务、需要隔离 | 需要频繁通信、共享数据的任务 |

### 进程创建示例
```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int shared_var = 100;
    printf("父进程：shared_var = %d, 地址 = %p\n", shared_var, &shared_var);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子进程
        shared_var = 200;  // 修改的是副本，不影响父进程
        printf("子进程：shared_var = %d, 地址 = %p\n", shared_var, &shared_var);
    } else {
        // 父进程
        wait(NULL);  // 等待子进程结束
        printf("父进程：shared_var = %d（未受影响）\n", shared_var);
    }
    
    return 0;
}
// 输出：
// 父进程：shared_var = 100, 地址 = 0x7ffc1234abcd
// 子进程：shared_var = 200, 地址 = 0x7ffc1234abcd （地址相同但内容独立！）
// 父进程：shared_var = 100（未受影响）
```

### 线程创建示例
```c
#include <stdio.h>
#include <pthread.h>

int shared_var = 100;  // 全局变量，所有线程共享

void *thread_func(void *arg) {
    shared_var = 200;  // 直接修改共享变量
    printf("线程：shared_var = %d, 地址 = %p\n", shared_var, &shared_var);
    return NULL;
}

int main() {
    printf("主线程：shared_var = %d, 地址 = %p\n", shared_var, &shared_var);
    
    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, NULL);
    pthread_join(thread, NULL);
    
    printf("主线程：shared_var = %d（被修改了！）\n", shared_var);
    return 0;
}
// 输出：
// 主线程：shared_var = 100, 地址 = 0x601040
// 线程：shared_var = 200, 地址 = 0x601040 （地址相同，内容共享！）
// 主线程：shared_var = 200（被修改了！）
```

### 何时使用进程？何时使用线程？

**使用进程的场景：**
- ✅ 需要强隔离性和安全性（如浏览器的沙箱）
- ✅ 不需要频繁通信
- ✅ 可以容忍较高的创建和切换开销
- ✅ 希望利用多机器的资源（分布式系统）
- ✅ 需要不同的权限级别

**使用线程的场景：**
- ✅ 需要频繁共享数据
- ✅ 需要低延迟的通信
- ✅ 资源受限（内存、CPU）
- ✅ 需要快速创建和销毁
- ✅ 任务之间紧密耦合

## POSIX 线程 (pthread)

### pthread 简介

POSIX 线程（通常称为 pthread）是 POSIX（Portable Operating System Interface）标准定义的线程 API，在 Linux、Unix、macOS 等系统上广泛使用。

**编译 pthread 程序：**
```bash
# 需要链接 pthread 库
gcc -pthread program.c -o program
# 或者
gcc program.c -o program -lpthread
```

### 线程的生命周期

```
线程生命周期状态图：

    创建(pthread_create)
          ↓
    ┌──────────┐
    │  新建    │
    │ (New)    │
    └──────────┘
          ↓
    ┌──────────┐
    │  就绪    │ ←──┐
    │ (Ready)  │    │ 被抢占
    └──────────┘    │
          ↓         │
    ┌──────────┐    │
    │  运行    │────┘
    │(Running) │
    └──────────┘
          ↓
    ┌──────────┐
    │  终止    │
    │(Terminated)
    └──────────┘
          ↓
    pthread_join 或 pthread_detach
          ↓
    资源回收
```

### pthread_create - 创建线程

**函数原型：**
```c
#include <pthread.h>

int pthread_create(
    pthread_t *thread,                    // 输出：线程 ID
    const pthread_attr_t *attr,          // 线程属性（NULL 使用默认）
    void *(*start_routine)(void *),      // 线程函数
    void *arg                             // 传递给线程函数的参数
);
// 返回值：成功返回 0，失败返回错误码
```

**详细示例：**
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// 线程参数结构体
typedef struct {
    int id;
    char *name;
    int iterations;
} WorkerArgs;

// 线程函数
void *worker(void *arg) {
    WorkerArgs *ctx = (WorkerArgs *)arg;
    
    printf("线程 %d (%s) 开始执行，线程ID: %lu\n", 
           ctx->id, ctx->name, pthread_self());
    
    for (int i = 0; i < ctx->iterations; i++) {
        printf("  [线程 %d] 迭代 %d/%d\n", ctx->id, i+1, ctx->iterations);
        sleep(1);  // 模拟工作
    }
    
    printf("线程 %d (%s) 完成\n", ctx->id, ctx->name);
    
    // 返回值可以通过 pthread_join 获取
    int *result = malloc(sizeof(int));
    *result = ctx->id * 100;
    return result;
}

int main(void) {
    pthread_t threads[3];
    WorkerArgs args[3] = {
        {1, "Worker-A", 3},
        {2, "Worker-B", 2},
        {3, "Worker-C", 4}
    };
    
    // 创建 3 个线程
    for (int i = 0; i < 3; i++) {
        int ret = pthread_create(&threads[i], NULL, worker, &args[i]);
        if (ret != 0) {
            fprintf(stderr, "创建线程 %d 失败，错误码: %d\n", i, ret);
            return 1;
        }
        printf("主线程：已创建线程 %d\n", i);
    }
    
    printf("主线程：所有线程已创建，等待完成...\n");
    
    // 等待所有线程完成
    for (int i = 0; i < 3; i++) {
        void *result;
        pthread_join(threads[i], &result);
        printf("主线程：线程 %d 返回值 = %d\n", i, *(int*)result);
        free(result);
    }
    
    printf("主线程：所有线程已完成\n");
    return 0;
}
```

**常见错误和注意事项：**

```c
// ❌ 错误1：传递栈上的局部变量地址（生命周期问题）
void bad_example1() {
    for (int i = 0; i < 10; i++) {
        pthread_t thread;
        // 危险！i 的地址在所有线程间共享，且循环可能在线程使用前就修改了 i
        pthread_create(&thread, NULL, worker, &i);
    }
}

// ✅ 正确：为每个线程分配独立的参数
void good_example1() {
    pthread_t threads[10];
    int *args[10];
    
    for (int i = 0; i < 10; i++) {
        args[i] = malloc(sizeof(int));
        *args[i] = i;
        pthread_create(&threads[i], NULL, worker, args[i]);
    }
    
    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
        free(args[i]);
    }
}

// ❌ 错误2：没有检查返回值
void bad_example2() {
    pthread_t thread;
    pthread_create(&thread, NULL, worker, NULL);  // 可能失败但没检查
}

// ✅ 正确：检查返回值
void good_example2() {
    pthread_t thread;
    int ret = pthread_create(&thread, NULL, worker, NULL);
    if (ret != 0) {
        fprintf(stderr, "pthread_create 失败: %s\n", strerror(ret));
        exit(1);
    }
}
```

### pthread_join - 等待线程结束

**函数原型：**
```c
int pthread_join(
    pthread_t thread,      // 要等待的线程 ID
    void **retval         // 输出：线程返回值（可以为 NULL）
);
// 返回值：成功返回 0，失败返回错误码
```

**特性：**
- 阻塞调用：调用线程会等待目标线程结束
- 资源回收：回收线程占用的系统资源
- 获取返回值：可以获取线程函数的返回值
- 只能调用一次：对同一线程多次调用 `pthread_join` 是未定义行为

**详细示例：**
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// 模拟下载文件的线程
void *download_file(void *arg) {
    char *filename = (char *)arg;
    printf("开始下载: %s\n", filename);
    
    // 模拟下载过程
    for (int i = 1; i <= 5; i++) {
        printf("  %s: %d%%\n", filename, i * 20);
        sleep(1);
    }
    
    printf("下载完成: %s\n", filename);
    
    // 返回下载的字节数（模拟）
    long *bytes = malloc(sizeof(long));
    *bytes = 1024 * 1024 * (rand() % 100);
    return bytes;
}

int main() {
    pthread_t threads[3];
    char *files[] = {"file1.txt", "file2.txt", "file3.txt"};
    
    // 创建下载线程
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, download_file, files[i]);
    }
    
    printf("所有下载已启动，等待完成...\n\n");
    
    // 等待所有下载完成
    long total_bytes = 0;
    for (int i = 0; i < 3; i++) {
        void *result;
        pthread_join(threads[i], &result);
        
        long bytes = *(long *)result;
        printf("线程 %d 下载了 %ld 字节\n", i, bytes);
        total_bytes += bytes;
        
        free(result);
    }
    
    printf("\n总共下载: %ld 字节\n", total_bytes);
    return 0;
}
```

### pthread_detach - 分离线程

**函数原型：**
```c
int pthread_detach(pthread_t thread);
// 返回值：成功返回 0，失败返回错误码
```

**特性：**
- 自动回收：线程结束后自动释放资源，无需 `pthread_join`
- 不可逆：分离后不能再 join
- 不能获取返回值
- 适用场景：不关心线程返回值的"后台任务"

**可结合（Joinable）vs 分离（Detached）对比：**

| 特性 | Joinable（默认） | Detached |
|------|-----------------|----------|
| 资源回收 | 需要显式 pthread_join | 自动回收 |
| 返回值 | 可以获取 | 无法获取 |
| 父线程等待 | 可以等待 | 无法等待 |
| 资源泄漏风险 | 高（如果忘记 join） | 低 |
| 适用场景 | 需要返回值或同步 | 后台任务 |

**示例：**
```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *background_task(void *arg) {
    int task_id = *(int *)arg;
    printf("后台任务 %d 开始\n", task_id);
    sleep(3);
    printf("后台任务 %d 完成\n", task_id);
    return NULL;
}

int main() {
    pthread_t thread;
    int arg = 42;
    
    pthread_create(&thread, NULL, background_task, &arg);
    
    // 分离线程：不关心它何时结束
    pthread_detach(thread);
    
    printf("主线程继续执行其他工作...\n");
    sleep(1);
    printf("主线程完成，但后台任务可能还在运行\n");
    
    // 为了演示，这里等待一下让后台任务完成
    sleep(3);
    
    return 0;
}
```

**创建时就设置为 detached：**
```c
void create_detached_thread() {
    pthread_t thread;
    pthread_attr_t attr;
    
    // 初始化线程属性
    pthread_attr_init(&attr);
    
    // 设置为 detached 状态
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    // 创建线程
    pthread_create(&thread, &attr, thread_func, NULL);
    
    // 清理属性
    pthread_attr_destroy(&attr);
}
```

### pthread_exit - 退出线程

**函数原型：**
```c
void pthread_exit(void *retval);
// 注意：这个函数不返回
```

**特性：**
- 在线程函数中任何地方调用都可以退出线程
- 类似于 `return`，但可以在深层函数中调用
- 会执行清理处理程序（cleanup handlers）

**示例：**
```c
#include <stdio.h>
#include <pthread.h>

void deep_function(int depth) {
    if (depth > 5) {
        printf("深度过大，退出线程\n");
        pthread_exit((void *)99);  // 退出整个线程
    }
    printf("深度: %d\n", depth);
    deep_function(depth + 1);
}

void *thread_func(void *arg) {
    printf("线程开始\n");
    deep_function(1);
    printf("这行不会执行\n");
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, NULL);
    
    void *result;
    pthread_join(thread, &result);
    printf("线程退出码: %ld\n", (long)result);  // 99
    
    return 0;
}
```

### 线程属性（pthread_attr_t）

线程属性允许精细控制线程的行为：

```c
#include <stdio.h>
#include <pthread.h>

void demonstrate_thread_attributes() {
    pthread_t thread;
    pthread_attr_t attr;
    
    // 1. 初始化属性对象
    pthread_attr_init(&attr);
    
    // 2. 设置栈大小（默认通常是 8MB）
    size_t stack_size = 4 * 1024 * 1024;  // 4MB
    pthread_attr_setstacksize(&attr, stack_size);
    
    // 3. 设置分离状态
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    // 4. 设置调度策略（需要 root 权限）
    // pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    
    // 5. 设置调度优先级
    // struct sched_param param;
    // param.sched_priority = 50;
    // pthread_attr_setschedparam(&attr, &param);
    
    // 6. 创建线程
    pthread_create(&thread, &attr, thread_func, NULL);
    
    // 7. 销毁属性对象（不影响已创建的线程）
    pthread_attr_destroy(&attr);
}

// 查询线程属性
void query_thread_attributes(pthread_t thread) {
    pthread_attr_t attr;
    
    // 获取线程属性
    pthread_getattr_np(thread, &attr);  // np = non-portable
    
    // 查询栈大小
    size_t stack_size;
    pthread_attr_getstacksize(&attr, &stack_size);
    printf("栈大小: %zu 字节\n", stack_size);
    
    // 查询分离状态
    int detach_state;
    pthread_attr_getdetachstate(&attr, &detach_state);
    printf("分离状态: %s\n", 
           detach_state == PTHREAD_CREATE_DETACHED ? "detached" : "joinable");
    
    pthread_attr_destroy(&attr);
}
```

### 线程局部存储（Thread-Local Storage, TLS）

每个线程都有自己独立的变量副本：

```c
#include <stdio.h>
#include <pthread.h>

// 方法1：使用 __thread 关键字（GCC 扩展，最简单）
__thread int thread_local_var = 0;

void *thread_func1(void *arg) {
    int id = *(int *)arg;
    thread_local_var = id * 100;
    
    printf("线程 %d: thread_local_var = %d\n", id, thread_local_var);
    return NULL;
}

// 方法2：使用 pthread_key_t（POSIX 标准，更灵活）
pthread_key_t tls_key;

void tls_destructor(void *data) {
    printf("清理线程局部数据: %d\n", *(int *)data);
    free(data);
}

void *thread_func2(void *arg) {
    int id = *(int *)arg;
    
    // 为这个线程分配数据
    int *data = malloc(sizeof(int));
    *data = id * 200;
    pthread_setspecific(tls_key, data);
    
    // 读取线程局部数据
    int *retrieved = pthread_getspecific(tls_key);
    printf("线程 %d: TLS data = %d\n", id, *retrieved);
    
    return NULL;
}

int main() {
    // 方法1 示例
    printf("=== 方法1: __thread ===\n");
    pthread_t threads1[3];
    int args1[] = {1, 2, 3};
    
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads1[i], NULL, thread_func1, &args1[i]);
    }
    for (int i = 0; i < 3; i++) {
        pthread_join(threads1[i], NULL);
    }
    
    // 方法2 示例
    printf("\n=== 方法2: pthread_key_t ===\n");
    pthread_key_create(&tls_key, tls_destructor);  // 创建 key，设置析构函数
    
    pthread_t threads2[3];
    int args2[] = {4, 5, 6};
    
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads2[i], NULL, thread_func2, &args2[i]);
    }
    for (int i = 0; i < 3; i++) {
        pthread_join(threads2[i], NULL);
    }
    
    pthread_key_delete(tls_key);  // 删除 key
    
    return 0;
}
```

### 线程取消（Thread Cancellation）

允许一个线程请求另一个线程终止：

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void cleanup_handler(void *arg) {
    printf("清理函数被调用，参数: %s\n", (char *)arg);
}

void *cancelable_thread(void *arg) {
    // 设置取消类型
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    // PTHREAD_CANCEL_DEFERRED: 在取消点才取消（默认，更安全）
    // PTHREAD_CANCEL_ASYNCHRONOUS: 立即取消（危险）
    
    // 注册清理处理程序
    pthread_cleanup_push(cleanup_handler, "资源A");
    pthread_cleanup_push(cleanup_handler, "资源B");
    
    printf("线程开始执行\n");
    
    for (int i = 0; i < 10; i++) {
        printf("  迭代 %d\n", i);
        sleep(1);  // sleep 是取消点
        // pthread_testcancel();  // 手动设置取消点
    }
    
    printf("线程正常完成\n");
    
    // 清理处理程序出栈（0 = 不执行，1 = 执行）
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, cancelable_thread, NULL);
    
    sleep(3);
    printf("主线程：请求取消线程\n");
    pthread_cancel(thread);
    
    void *result;
    pthread_join(thread, &result);
    
    if (result == PTHREAD_CANCELED) {
        printf("主线程：线程已被取消\n");
    } else {
        printf("主线程：线程正常结束\n");
    }
    
    return 0;
}
```

## 同步原语

在多线程编程中，**同步原语**（Synchronization Primitives）是用于协调多个线程访问共享资源的基本工具。没有适当的同步，并发访问会导致数据竞争和不一致。

### 为什么需要同步？

考虑这个简单的例子：

```c
// 没有同步的计数器（错误示例）
#include <stdio.h>
#include <pthread.h>

int counter = 0;

void *increment(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        counter++;  // 看起来是原子操作，实际不是！
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("Counter = %d (预期: 2000000)\n", counter);
    // 实际输出可能是：1234567（每次运行都不同！）
    return 0;
}
```

**为什么会出错？**

```
counter++ 实际上是三个操作（非原子）：

1. 从内存读取 counter 的值到寄存器    LOAD
2. 将寄存器的值加 1                   ADD
3. 将寄存器的值写回内存               STORE

并发执行时的竞态条件：

时间 →
线程1: LOAD(0) → ADD(1) → [被抢占] → STORE(1)
线程2:              LOAD(0) → ADD(1) → STORE(1)

结果：两次递增，但 counter 只变成了 1（而不是 2）
这就是"丢失更新"（Lost Update）问题
```

### 互斥锁（Mutex - Mutual Exclusion）

互斥锁是最基本的同步原语，确保同一时刻只有一个线程可以访问临界区。

#### 基本使用

```c
#include <stdio.h>
#include <pthread.h>

int counter = 0;
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void *increment_safe(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        pthread_mutex_lock(&counter_mutex);    // 加锁
        counter++;                              // 临界区
        pthread_mutex_unlock(&counter_mutex);  // 解锁
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    
    pthread_create(&t1, NULL, increment_safe, NULL);
    pthread_create(&t2, NULL, increment_safe, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("Counter = %d (正确！)\n", counter);  // 输出：2000000
    
    pthread_mutex_destroy(&counter_mutex);
    return 0;
}
```

#### 互斥锁的工作原理

```
互斥锁状态机：

    ┌─────────────┐
    │  未锁定     │
    │ (Unlocked)  │
    └─────────────┘
          ↓ pthread_mutex_lock()
          ↓ (成功获取)
    ┌─────────────┐
    │  已锁定     │
    │  (Locked)   │
    │  所有者: T1 │
    └─────────────┘
          ↓ pthread_mutex_unlock()
          ↓
    ┌─────────────┐
    │  未锁定     │
    └─────────────┘

如果锁已被占用：
线程2 调用 pthread_mutex_lock() → 阻塞等待 → 锁可用时唤醒 → 获取锁
```

#### 互斥锁的初始化方式

```c
// 方法1：静态初始化（适用于全局锁）
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

// 方法2：动态初始化（可以设置属性）
pthread_mutex_t mutex2;
pthread_mutex_init(&mutex2, NULL);  // NULL = 默认属性

// 方法3：设置互斥锁属性
pthread_mutex_t mutex3;
pthread_mutexattr_t attr;
pthread_mutexattr_init(&attr);

// 设置类型：
// - PTHREAD_MUTEX_NORMAL: 默认，不检测死锁
// - PTHREAD_MUTEX_ERRORCHECK: 检测死锁，返回错误
// - PTHREAD_MUTEX_RECURSIVE: 可递归锁（同一线程可多次加锁）
pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

pthread_mutex_init(&mutex3, &attr);
pthread_mutexattr_destroy(&attr);

// 使用完毕后销毁
pthread_mutex_destroy(&mutex2);
pthread_mutex_destroy(&mutex3);
```

#### 递归锁示例

```c
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t recursive_mutex;

void recursive_function(int depth) {
    pthread_mutex_lock(&recursive_mutex);
    
    printf("深度: %d\n", depth);
    
    if (depth < 5) {
        recursive_function(depth + 1);  // 再次获取同一个锁
    }
    
    pthread_mutex_unlock(&recursive_mutex);
}

int main() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&recursive_mutex, &attr);
    
    recursive_function(0);
    
    pthread_mutex_destroy(&recursive_mutex);
    pthread_mutexattr_destroy(&attr);
    return 0;
}
```

#### 死锁示例和避免

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_b = PTHREAD_MUTEX_INITIALIZER;

// ❌ 错误：可能导致死锁
void *thread1_bad(void *arg) {
    pthread_mutex_lock(&mutex_a);
    printf("线程1: 获取了 mutex_a\n");
    sleep(1);  // 增加死锁概率
    
    printf("线程1: 等待 mutex_b...\n");
    pthread_mutex_lock(&mutex_b);  // 线程2 可能持有 mutex_b
    
    printf("线程1: 获取了 mutex_b\n");
    
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
    return NULL;
}

void *thread2_bad(void *arg) {
    pthread_mutex_lock(&mutex_b);
    printf("线程2: 获取了 mutex_b\n");
    sleep(1);
    
    printf("线程2: 等待 mutex_a...\n");
    pthread_mutex_lock(&mutex_a);  // 线程1 可能持有 mutex_a → 死锁！
    
    printf("线程2: 获取了 mutex_a\n");
    
    pthread_mutex_unlock(&mutex_a);
    pthread_mutex_unlock(&mutex_b);
    return NULL;
}

// ✅ 正确：统一加锁顺序
void *thread1_good(void *arg) {
    // 总是先锁 a，再锁 b
    pthread_mutex_lock(&mutex_a);
    pthread_mutex_lock(&mutex_b);
    
    printf("线程1: 工作中...\n");
    
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
    return NULL;
}

void *thread2_good(void *arg) {
    // 总是先锁 a，再锁 b（顺序一致）
    pthread_mutex_lock(&mutex_a);
    pthread_mutex_lock(&mutex_b);
    
    printf("线程2: 工作中...\n");
    
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
    return NULL;
}

// ✅ 更好：使用 trylock 避免死锁
void *thread_trylock(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex_a);
        
        if (pthread_mutex_trylock(&mutex_b) == 0) {
            // 成功获取两个锁
            printf("线程: 工作中...\n");
            
            pthread_mutex_unlock(&mutex_b);
            pthread_mutex_unlock(&mutex_a);
            break;
        } else {
            // 获取 mutex_b 失败，释放 mutex_a 并重试
            pthread_mutex_unlock(&mutex_a);
            usleep(100);  // 短暂等待后重试
        }
    }
    return NULL;
}
```

#### 锁的粒度优化

```c
#include <stdio.h>
#include <pthread.h>

typedef struct {
    int data[1000];
    pthread_mutex_t mutex;
} DataStore;

// ❌ 粗粒度锁：整个函数都在锁内（效率低）
void process_data_coarse(DataStore *store) {
    pthread_mutex_lock(&store->mutex);
    
    // 耗时的本地计算（不需要锁保护）
    int local_sum = 0;
    for (int i = 0; i < 1000; i++) {
        local_sum += i * i;
    }
    
    // 修改共享数据
    store->data[0] = local_sum;
    
    pthread_mutex_unlock(&store->mutex);
}

// ✅ 细粒度锁：只保护必要的部分（效率高）
void process_data_fine(DataStore *store) {
    // 耗时的本地计算（在锁外）
    int local_sum = 0;
    for (int i = 0; i < 1000; i++) {
        local_sum += i * i;
    }
    
    // 只在修改共享数据时加锁
    pthread_mutex_lock(&store->mutex);
    store->data[0] = local_sum;
    pthread_mutex_unlock(&store->mutex);
}
```

### 读写锁（Read-Write Lock）

读写锁针对"读多写少"场景优化，允许多个读者同时访问，但写者独占访问。

```
读写锁的规则：
1. 多个线程可以同时持有读锁
2. 写锁是互斥的（与读锁和其他写锁都互斥）
3. 写者优先或读者优先（可配置）

状态转换：
         读者1      读者2      读者3
           ↓          ↓          ↓
    ┌──────────────────────────────┐
    │   多个读者可以同时访问        │
    └──────────────────────────────┘
                    ↓
              等待所有读者完成
                    ↓
    ┌──────────────────────────────┐
    │   写者独占访问               │
    └──────────────────────────────┘
```

**基本使用：**

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int value;
    pthread_rwlock_t rwlock;
} SharedData;

// 读者线程：只读取数据
void *reader(void *arg) {
    SharedData *data = (SharedData *)arg;
    
    for (int i = 0; i < 5; i++) {
        pthread_rwlock_rdlock(&data->rwlock);  // 获取读锁
        
        printf("  [读者 %lu] 读取: %d\n", pthread_self(), data->value);
        usleep(100000);  // 模拟读取操作
        
        pthread_rwlock_unlock(&data->rwlock);
        usleep(50000);
    }
    return NULL;
}

// 写者线程：修改数据
void *writer(void *arg) {
    SharedData *data = (SharedData *)arg;
    
    for (int i = 0; i < 3; i++) {
        pthread_rwlock_wrlock(&data->rwlock);  // 获取写锁
        
        data->value++;
        printf("[写者 %lu] 写入: %d\n", pthread_self(), data->value);
        usleep(200000);  // 模拟写入操作
        
        pthread_rwlock_unlock(&data->rwlock);
        usleep(300000);
    }
    return NULL;
}

int main() {
    SharedData data = { .value = 0 };
    pthread_rwlock_init(&data.rwlock, NULL);
    
    pthread_t readers[5], writers[2];
    
    // 创建 2 个写者和 5 个读者
    for (int i = 0; i < 2; i++) {
        pthread_create(&writers[i], NULL, writer, &data);
    }
    for (int i = 0; i < 5; i++) {
        pthread_create(&readers[i], NULL, reader, &data);
    }
    
    // 等待所有线程完成
    for (int i = 0; i < 2; i++) {
        pthread_join(writers[i], NULL);
    }
    for (int i = 0; i < 5; i++) {
        pthread_join(readers[i], NULL);
    }
    
    pthread_rwlock_destroy(&data.rwlock);
    printf("最终值: %d\n", data.value);
    return 0;
}
```

**读写锁 vs 互斥锁性能对比：**

```c
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define NUM_READERS 10
#define NUM_WRITERS 2
#define ITERATIONS 10000

typedef struct {
    int data;
    pthread_mutex_t mutex;
    pthread_rwlock_t rwlock;
} BenchmarkData;

// 使用互斥锁
void *reader_mutex(void *arg) {
    BenchmarkData *bd = arg;
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&bd->mutex);
        int value = bd->data;  // 读取
        pthread_mutex_unlock(&bd->mutex);
    }
    return NULL;
}

// 使用读写锁
void *reader_rwlock(void *arg) {
    BenchmarkData *bd = arg;
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_rwlock_rdlock(&bd->rwlock);
        int value = bd->data;  // 读取
        pthread_rwlock_unlock(&bd->rwlock);
    }
    return NULL;
}

double benchmark(void *(*reader_func)(void *), BenchmarkData *bd) {
    pthread_t readers[NUM_READERS];
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader_func, bd);
    }
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    double elapsed = (end.tv_sec - start.tv_sec) + 
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    return elapsed;
}

int main() {
    BenchmarkData bd = { .data = 0 };
    pthread_mutex_init(&bd.mutex, NULL);
    pthread_rwlock_init(&bd.rwlock, NULL);
    
    printf("测试 %d 个读者，每个读 %d 次\n", NUM_READERS, ITERATIONS);
    
    double time_mutex = benchmark(reader_mutex, &bd);
    printf("互斥锁耗时: %.3f 秒\n", time_mutex);
    
    double time_rwlock = benchmark(reader_rwlock, &bd);
    printf("读写锁耗时: %.3f 秒\n", time_rwlock);
    
    printf("性能提升: %.1f%%\n", (time_mutex - time_rwlock) / time_mutex * 100);
    
    pthread_mutex_destroy(&bd.mutex);
    pthread_rwlock_destroy(&bd.rwlock);
    return 0;
}
```

### 条件变量（Condition Variable）

条件变量用于线程间的通知机制，允许线程等待某个条件成立。

**关键概念：**
- 必须与互斥锁配合使用
- `pthread_cond_wait()` 会原子地解锁互斥锁并等待
- 被唤醒后会重新获取互斥锁
- 防止虚假唤醒：总是在循环中检查条件

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct {
    int value;
    bool ready;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} SharedResource;

// 生产者：准备数据并通知
void *producer(void *arg) {
    SharedResource *res = (SharedResource *)arg;
    
    sleep(2);  // 模拟准备数据
    
    pthread_mutex_lock(&res->mutex);
    
    res->value = 42;
    res->ready = true;
    printf("生产者：数据已准备好，value = %d\n", res->value);
    
    // 唤醒等待的消费者
    pthread_cond_signal(&res->cond);  // 唤醒一个线程
    // pthread_cond_broadcast(&res->cond);  // 唤醒所有线程
    
    pthread_mutex_unlock(&res->mutex);
    return NULL;
}

// 消费者：等待数据准备好
void *consumer(void *arg) {
    SharedResource *res = (SharedResource *)arg;
    
    pthread_mutex_lock(&res->mutex);
    
    printf("消费者：等待数据...\n");
    
    // 重要：在循环中检查条件（防止虚假唤醒）
    while (!res->ready) {
        // pthread_cond_wait 做三件事：
        // 1. 解锁 mutex
        // 2. 阻塞等待 cond
        // 3. 被唤醒后重新锁 mutex
        pthread_cond_wait(&res->cond, &res->mutex);
    }
    
    printf("消费者：收到数据，value = %d\n", res->value);
    
    pthread_mutex_unlock(&res->mutex);
    return NULL;
}

int main() {
    SharedResource res = {
        .value = 0,
        .ready = false,
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .cond = PTHREAD_COND_INITIALIZER
    };
    
    pthread_t prod, cons;
    
    pthread_create(&cons, NULL, consumer, &res);
    pthread_create(&prod, NULL, producer, &res);
    
    pthread_join(cons, NULL);
    pthread_join(prod, NULL);
    
    pthread_mutex_destroy(&res.mutex);
    pthread_cond_destroy(&res.cond);
    return 0;
}
```

**条件变量的工作流程：**

```
消费者线程：                      生产者线程：

pthread_mutex_lock()
  ↓
检查条件 (ready == false)
  ↓
pthread_cond_wait()
  ├─ 解锁 mutex ─────────→    pthread_mutex_lock()
  ├─ 进入等待队列                    ↓
  │                              准备数据
  │                                  ↓
  │                              ready = true
  │                                  ↓
  │                         pthread_cond_signal()
  ←─ 被唤醒 ←────────────────┘       ↓
  ├─ 重新获取 mutex          pthread_mutex_unlock()
  ↓
再次检查条件 (ready == true)
  ↓
处理数据
  ↓
pthread_mutex_unlock()
```

**多消费者示例（广播）：**

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int value;
    bool ready;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} BroadcastData;

void *consumer_multi(void *arg) {
    BroadcastData *data = arg;
    long id = (long)pthread_self();
    
    pthread_mutex_lock(&data->mutex);
    
    printf("消费者 %ld: 等待...\n", id);
    
    while (!data->ready) {
        pthread_cond_wait(&data->cond, &data->mutex);
    }
    
    printf("消费者 %ld: 收到数据 %d\n", id, data->value);
    
    pthread_mutex_unlock(&data->mutex);
    return NULL;
}

void *producer_broadcast(void *arg) {
    BroadcastData *data = arg;
    
    sleep(2);
    
    pthread_mutex_lock(&data->mutex);
    
    data->value = 100;
    data->ready = true;
    printf("生产者：广播通知所有消费者\n");
    
    pthread_cond_broadcast(&data->cond);  // 唤醒所有等待的线程
    
    pthread_mutex_unlock(&data->mutex);
    return NULL;
}

int main() {
    BroadcastData data = {
        .value = 0,
        .ready = false,
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .cond = PTHREAD_COND_INITIALIZER
    };
    
    pthread_t consumers[5], producer;
    
    // 创建多个消费者
    for (int i = 0; i < 5; i++) {
        pthread_create(&consumers[i], NULL, consumer_multi, &data);
    }
    
    // 创建一个生产者
    pthread_create(&producer, NULL, producer_broadcast, &data);
    
    // 等待所有线程
    for (int i = 0; i < 5; i++) {
        pthread_join(consumers[i], NULL);
    }
    pthread_join(producer, NULL);
    
    pthread_mutex_destroy(&data.mutex);
    pthread_cond_destroy(&data.cond);
    return 0;
}
```

### 信号量（Semaphore）

信号量是一个计数器，用于控制对有限资源的访问。

**类型：**
- **二值信号量（Binary Semaphore）**：只有 0 和 1，类似互斥锁
- **计数信号量（Counting Semaphore）**：可以有多个资源

```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_CONNECTIONS 3

sem_t connection_pool;

void *client(void *arg) {
    int id = *(int *)arg;
    
    printf("客户端 %d: 请求连接...\n", id);
    
    // 等待可用连接（P 操作，减 1）
    sem_wait(&connection_pool);
    
    printf("客户端 %d: 获得连接，处理中...\n", id);
    sleep(2);  // 模拟使用连接
    printf("客户端 %d: 释放连接\n", id);
    
    // 释放连接（V 操作，加 1）
    sem_post(&connection_pool);
    
    free(arg);
    return NULL;
}

int main() {
    // 初始化信号量，初始值为 MAX_CONNECTIONS
    sem_init(&connection_pool, 0, MAX_CONNECTIONS);
    
    pthread_t clients[10];
    
    // 创建 10 个客户端，但只有 3 个可以同时连接
    for (int i = 0; i < 10; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&clients[i], NULL, client, id);
        usleep(100000);  // 错开启动时间
    }
    
    for (int i = 0; i < 10; i++) {
        pthread_join(clients[i], NULL);
    }
    
    sem_destroy(&connection_pool);
    printf("所有客户端处理完成\n");
    return 0;
}
```

**生产者-消费者（使用信号量）：**

```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

sem_t empty;  // 空槽位数量
sem_t full;   // 满槽位数量
pthread_mutex_t mutex;

void *producer_sem(void *arg) {
    int id = *(int *)arg;
    
    for (int i = 0; i < 10; i++) {
        int item = id * 100 + i;
        
        sem_wait(&empty);  // 等待空槽位
        pthread_mutex_lock(&mutex);
        
        buffer[in] = item;
        printf("生产者 %d: 生产 %d 到位置 %d\n", id, item, in);
        in = (in + 1) % BUFFER_SIZE;
        
        pthread_mutex_unlock(&mutex);
        sem_post(&full);  // 增加满槽位
        
        usleep(100000);
    }
    return NULL;
}

void *consumer_sem(void *arg) {
    int id = *(int *)arg;
    
    for (int i = 0; i < 10; i++) {
        sem_wait(&full);  // 等待满槽位
        pthread_mutex_lock(&mutex);
        
        int item = buffer[out];
        printf("  消费者 %d: 消费 %d 从位置 %d\n", id, item, out);
        out = (out + 1) % BUFFER_SIZE;
        
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);  // 增加空槽位
        
        usleep(150000);
    }
    return NULL;
}

int main() {
    sem_init(&empty, 0, BUFFER_SIZE);  // 初始全空
    sem_init(&full, 0, 0);             // 初始无数据
    pthread_mutex_init(&mutex, NULL);
    
    pthread_t producers[2], consumers[2];
    int ids[] = {1, 2};
    
    for (int i = 0; i < 2; i++) {
        pthread_create(&producers[i], NULL, producer_sem, &ids[i]);
        pthread_create(&consumers[i], NULL, consumer_sem, &ids[i]);
    }
    
    for (int i = 0; i < 2; i++) {
        pthread_join(producers[i], NULL);
        pthread_join(consumers[i], NULL);
    }
    
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    return 0;
}
```

### 屏障（Barrier）

屏障用于同步一组线程，所有线程都到达屏障点后才能继续执行。

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 5

pthread_barrier_t barrier;

void *worker_barrier(void *arg) {
    int id = *(int *)arg;
    
    // 阶段 1
    printf("线程 %d: 阶段 1 开始\n", id);
    sleep(id);  // 模拟不同的工作时间
    printf("线程 %d: 阶段 1 完成，等待其他线程...\n", id);
    
    pthread_barrier_wait(&barrier);  // 等待所有线程
    
    // 阶段 2（所有线程同时开始）
    printf("线程 %d: 阶段 2 开始\n", id);
    sleep(1);
    printf("线程 %d: 阶段 2 完成\n", id);
    
    return NULL;
}

int main() {
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
    
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];
    
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, worker_barrier, &ids[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_barrier_destroy(&barrier);
    return 0;
}
```

### 自旋锁（Spinlock）

自旋锁不会让线程休眠，而是忙等待（busy-waiting），适用于锁持有时间非常短的场景。

```c
#include <stdio.h>
#include <pthread.h>

pthread_spinlock_t spinlock;
int counter = 0;

void *increment_spin(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        pthread_spin_lock(&spinlock);
        counter++;
        pthread_spin_unlock(&spinlock);
    }
    return NULL;
}

int main() {
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
    
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment_spin, NULL);
    pthread_create(&t2, NULL, increment_spin, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("Counter: %d\n", counter);
    
    pthread_spin_destroy(&spinlock);
    return 0;
}
```

**互斥锁 vs 自旋锁：**

| 特性 | 互斥锁 | 自旋锁 |
|------|--------|--------|
| 等待方式 | 休眠（让出 CPU） | 忙等待（占用 CPU） |
| 适用场景 | 锁持有时间较长 | 锁持有时间极短 |
| CPU 占用 | 低 | 高 |
| 上下文切换 | 有 | 无 |
| 延迟 | 较高 | 极低 |

## 原子操作

### 什么是原子操作？

**原子操作**（Atomic Operation）是不可分割的操作，在执行过程中不会被其他线程打断。原子操作是无锁编程的基础。

**为什么需要原子操作？**

```c
// 普通的递增操作不是原子的
int counter = 0;

// counter++ 实际上是三个步骤：
// 1. 读取 counter 的值到寄存器
// 2. 寄存器值 +1
// 3. 写回 counter

// 在多线程环境下：
线程1: LOAD(0) -> ADD(1) -> [被中断]
线程2:              LOAD(0) -> ADD(1) -> STORE(1)
线程1:                                    -> STORE(1)
结果: counter = 1（而不是期望的 2）
```

### C11 原子类型（stdatomic.h）

C11 标准引入了原子类型和操作：

```c
#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>

// 原子整数类型
atomic_int atomic_counter = ATOMIC_VAR_INIT(0);

// 普通整数（用于对比）
int normal_counter = 0;

void *increment_atomic(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        atomic_fetch_add(&atomic_counter, 1);  // 原子递增
    }
    return NULL;
}

void *increment_normal(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        normal_counter++;  // 非原子递增
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    
    // 测试原子操作
    printf("=== 原子操作测试 ===\n");
    pthread_create(&t1, NULL, increment_atomic, NULL);
    pthread_create(&t2, NULL, increment_atomic, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("原子计数器: %d (预期: 2000000) ✓\n", atomic_counter);
    
    // 测试普通操作
    printf("\n=== 普通操作测试 ===\n");
    pthread_create(&t1, NULL, increment_normal, NULL);
    pthread_create(&t2, NULL, increment_normal, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("普通计数器: %d (预期: 2000000) ✗\n", normal_counter);
    
    return 0;
}
```

### 常用原子类型

```c
#include <stdatomic.h>

// 原子整数类型
atomic_bool     // 原子布尔
atomic_char     // 原子字符
atomic_short    // 原子短整型
atomic_int      // 原子整型
atomic_long     // 原子长整型
atomic_llong    // 原子长长整型

// 无符号版本
atomic_uchar, atomic_ushort, atomic_uint, atomic_ulong, atomic_ullong

// 原子指针
atomic_intptr_t   // 原子整数指针
atomic_uintptr_t  // 原子无符号整数指针

// 自定义原子类型
_Atomic int custom_atomic_int;
_Atomic struct MyStruct *atomic_ptr;
```

### 原子操作函数

```c
#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>

void demonstrate_atomic_operations() {
    atomic_int x = ATOMIC_VAR_INIT(100);
    
    // 1. atomic_load: 原子读取
    int value = atomic_load(&x);
    printf("读取值: %d\n", value);
    
    // 2. atomic_store: 原子写入
    atomic_store(&x, 200);
    printf("写入后: %d\n", atomic_load(&x));
    
    // 3. atomic_exchange: 原子交换（返回旧值）
    int old = atomic_exchange(&x, 300);
    printf("交换: 旧值=%d, 新值=%d\n", old, atomic_load(&x));
    
    // 4. atomic_compare_exchange_strong: CAS（Compare-And-Swap）
    int expected = 300;
    bool success = atomic_compare_exchange_strong(&x, &expected, 400);
    if (success) {
        printf("CAS 成功: %d -> 400\n", expected);
    } else {
        printf("CAS 失败: 期望=%d, 实际=%d\n", expected, atomic_load(&x));
    }
    
    // 5. atomic_fetch_add: 原子加法（返回旧值）
    old = atomic_fetch_add(&x, 50);
    printf("加法: 旧值=%d, 新值=%d\n", old, atomic_load(&x));
    
    // 6. atomic_fetch_sub: 原子减法
    old = atomic_fetch_sub(&x, 30);
    printf("减法: 旧值=%d, 新值=%d\n", old, atomic_load(&x));
    
    // 7. atomic_fetch_or: 原子按位或
    atomic_store(&x, 0b1010);
    old = atomic_fetch_or(&x, 0b0101);
    printf("按位或: 旧值=0b%04b, 新值=0b%04b\n", old, atomic_load(&x));
    
    // 8. atomic_fetch_and: 原子按位与
    atomic_store(&x, 0b1111);
    old = atomic_fetch_and(&x, 0b1100);
    printf("按位与: 旧值=0b%04b, 新值=0b%04b\n", old, atomic_load(&x));
    
    // 9. atomic_fetch_xor: 原子按位异或
    atomic_store(&x, 0b1010);
    old = atomic_fetch_xor(&x, 0b0110);
    printf("按位异或: 旧值=0b%04b, 新值=0b%04b\n", old, atomic_load(&x));
}
```

### CAS（Compare-And-Swap）详解

CAS 是最重要的原子操作，是无锁编程的基石：

```c
#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>

// CAS 的伪代码逻辑：
// bool CAS(int *ptr, int expected, int new_value) {
//     if (*ptr == expected) {
//         *ptr = new_value;
//         return true;
//     } else {
//         return false;
//     }
// }

atomic_int shared_value = ATOMIC_VAR_INIT(0);

// 使用 CAS 实现线程安全的递增
void *safe_increment(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        int current, next;
        do {
            current = atomic_load(&shared_value);
            next = current + 1;
            // 如果 shared_value == current，则设置为 next
            // 否则 current 会被更新为 shared_value 的当前值，继续重试
        } while (!atomic_compare_exchange_weak(&shared_value, &current, next));
    }
    return NULL;
}

// strong vs weak CAS
void compare_cas_types() {
    atomic_int x = ATOMIC_VAR_INIT(10);
    int expected = 10;
    
    // atomic_compare_exchange_strong:
    // - 只有在值不匹配时才失败
    // - 适用于大多数场景
    if (atomic_compare_exchange_strong(&x, &expected, 20)) {
        printf("Strong CAS 成功\n");
    }
    
    expected = 20;
    // atomic_compare_exchange_weak:
    // - 即使值匹配也可能失败（spurious failure）
    // - 更快，适用于循环中
    if (atomic_compare_exchange_weak(&x, &expected, 30)) {
        printf("Weak CAS 成功\n");
    }
}

int main() {
    pthread_t t1, t2;
    
    pthread_create(&t1, NULL, safe_increment, NULL);
    pthread_create(&t2, NULL, safe_increment, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("最终值: %d (预期: 2000000)\n", atomic_load(&shared_value));
    
    printf("\n");
    compare_cas_types();
    
    return 0;
}
```

### 原子操作实现自旋锁

```c
#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>

typedef struct {
    atomic_flag flag;
} spinlock_t;

void spinlock_init(spinlock_t *lock) {
    atomic_flag_clear(&lock->flag);
}

void spinlock_lock(spinlock_t *lock) {
    // 持续尝试设置标志，直到成功（之前是 clear 状态）
    while (atomic_flag_test_and_set(&lock->flag)) {
        // 忙等待（自旋）
        // 可选优化：
        // __builtin_ia32_pause();  // x86 的 PAUSE 指令
        // sched_yield();           // 让出 CPU
    }
}

void spinlock_unlock(spinlock_t *lock) {
    atomic_flag_clear(&lock->flag);
}

// 使用示例
spinlock_t my_lock;
int shared_counter = 0;

void *worker_spinlock(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        spinlock_lock(&my_lock);
        shared_counter++;
        spinlock_unlock(&my_lock);
    }
    return NULL;
}

int main() {
    spinlock_init(&my_lock);
    
    pthread_t t1, t2;
    pthread_create(&t1, NULL, worker_spinlock, NULL);
    pthread_create(&t2, NULL, worker_spinlock, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("Counter: %d\n", shared_counter);
    return 0;
}
```

### GCC/Clang 内建原子函数

在 C11 之前，GCC 和 Clang 提供了内建原子函数：

```c
#include <stdio.h>

void demonstrate_gcc_builtins() {
    int x = 0;
    
    // 旧版（已弃用，但仍广泛使用）
    __sync_fetch_and_add(&x, 1);      // x += 1，返回旧值
    __sync_fetch_and_sub(&x, 1);      // x -= 1
    __sync_fetch_and_or(&x, 1);       // x |= 1
    __sync_fetch_and_and(&x, 1);      // x &= 1
    __sync_fetch_and_xor(&x, 1);      // x ^= 1
    __sync_bool_compare_and_swap(&x, 0, 1);  // CAS，返回 bool
    __sync_val_compare_and_swap(&x, 1, 2);   // CAS，返回旧值
    __sync_synchronize();              // 内存屏障
    
    // 新版（推荐使用，更灵活）
    __atomic_load_n(&x, __ATOMIC_SEQ_CST);
    __atomic_store_n(&x, 42, __ATOMIC_SEQ_CST);
    __atomic_fetch_add(&x, 1, __ATOMIC_SEQ_CST);
    __atomic_fetch_sub(&x, 1, __ATOMIC_SEQ_CST);
    
    int expected = 42;
    __atomic_compare_exchange_n(&x, &expected, 100,
                                 0,  // weak=0, strong=1
                                 __ATOMIC_SEQ_CST,
                                 __ATOMIC_SEQ_CST);
    
    __atomic_thread_fence(__ATOMIC_SEQ_CST);  // 内存屏障
}
```

### 无锁数据结构：无锁栈

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

typedef struct {
    _Atomic(Node *) head;
} LockFreeStack;

void stack_init(LockFreeStack *stack) {
    atomic_store(&stack->head, NULL);
}

void stack_push(LockFreeStack *stack, int data) {
    Node *new_node = malloc(sizeof(Node));
    new_node->data = data;
    
    Node *old_head;
    do {
        old_head = atomic_load(&stack->head);
        new_node->next = old_head;
        // CAS: 如果 head 仍然是 old_head，则更新为 new_node
    } while (!atomic_compare_exchange_weak(&stack->head, &old_head, new_node));
}

bool stack_pop(LockFreeStack *stack, int *data) {
    Node *old_head;
    do {
        old_head = atomic_load(&stack->head);
        if (old_head == NULL) {
            return false;  // 栈空
        }
        // CAS: 如果 head 仍然是 old_head，则更新为 old_head->next
    } while (!atomic_compare_exchange_weak(&stack->head, &old_head, old_head->next));
    
    *data = old_head->data;
    free(old_head);  // 注意：这里有 ABA 问题！
    return true;
}

// 测试
void *pusher(void *arg) {
    LockFreeStack *stack = arg;
    for (int i = 0; i < 10000; i++) {
        stack_push(stack, i);
    }
    return NULL;
}

void *popper(void *arg) {
    LockFreeStack *stack = arg;
    int data;
    for (int i = 0; i < 10000; i++) {
        while (!stack_pop(stack, &data)) {
            // 重试
        }
    }
    return NULL;
}

int main() {
    LockFreeStack stack;
    stack_init(&stack);
    
    pthread_t pushers[2], poppers[2];
    
    for (int i = 0; i < 2; i++) {
        pthread_create(&pushers[i], NULL, pusher, &stack);
    }
    for (int i = 0; i < 2; i++) {
        pthread_create(&poppers[i], NULL, popper, &stack);
    }
    
    for (int i = 0; i < 2; i++) {
        pthread_join(pushers[i], NULL);
        pthread_join(poppers[i], NULL);
    }
    
    printf("无锁栈测试完成\n");
    return 0;
}
```

### ABA 问题

ABA 问题是无锁编程中的经典问题：

```
初始状态: head -> A -> B -> C

线程1: 读取 head = A，准备执行 CAS(head, A, B)
       [被中断]

线程2: pop A  (head = B)
       pop B  (head = C)
       push A (head = A)  // A 被重新分配到相同地址！
       
线程1: 恢复执行 CAS(head, A, B)
       成功！（因为 head 确实等于 A）
       但实际上 A 已经不是原来的 A 了！
       结果：C 丢失了！

解决方案：
1. 使用版本号（ABA -> A1BA2）
2. 使用垃圾回收或延迟释放
3. 使用双字 CAS（DWCAS）
4. 使用 Hazard Pointers
```

**使用版本号解决 ABA：**

```c
#include <stdatomic.h>
#include <stdint.h>

typedef struct {
    Node *ptr;
    uint64_t version;
} VersionedPointer;

typedef struct {
    _Atomic VersionedPointer head;
} SafeLockFreeStack;

void safe_stack_push(SafeLockFreeStack *stack, int data) {
    Node *new_node = malloc(sizeof(Node));
    new_node->data = data;
    
    VersionedPointer old_head, new_head;
    do {
        old_head = atomic_load(&stack->head);
        new_node->next = old_head.ptr;
        new_head.ptr = new_node;
        new_head.version = old_head.version + 1;  // 递增版本号
    } while (!atomic_compare_exchange_weak(&stack->head, &old_head, new_head));
}
```

### 原子操作的性能考虑

```c
#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>
#include <time.h>

#define ITERATIONS 10000000

// 测试不同同步方式的性能
double benchmark(void *(*func)(void *), void *arg) {
    pthread_t threads[2];
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    pthread_create(&threads[0], NULL, func, arg);
    pthread_create(&threads[1], NULL, func, arg);
    
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

// 1. 原子操作
atomic_int atomic_counter = 0;
void *test_atomic(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        atomic_fetch_add(&atomic_counter, 1);
    }
    return NULL;
}

// 2. 互斥锁
int mutex_counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *test_mutex(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);
        mutex_counter++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

// 3. 自旋锁
int spin_counter = 0;
pthread_spinlock_t spinlock;
void *test_spin(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_spin_lock(&spinlock);
        spin_counter++;
        pthread_spin_unlock(&spinlock);
    }
    return NULL;
}

int main() {
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
    
    printf("测试 %d 次迭代（2 个线程）\n\n", ITERATIONS);
    
    double time_atomic = benchmark(test_atomic, NULL);
    printf("原子操作: %.3f 秒\n", time_atomic);
    
    double time_mutex = benchmark(test_mutex, NULL);
    printf("互斥锁:   %.3f 秒 (%.1f%% 慢)\n", 
           time_mutex, (time_mutex/time_atomic - 1) * 100);
    
    double time_spin = benchmark(test_spin, NULL);
    printf("自旋锁:   %.3f 秒 (%.1f%% 慢)\n",
           time_spin, (time_spin/time_atomic - 1) * 100);
    
    pthread_spin_destroy(&spinlock);
    return 0;
}
```

**性能总结：**

| 方法 | 相对性能 | 适用场景 |
|------|----------|----------|
| 无同步 | 最快（但不安全） | 单线程或只读 |
| 原子操作 | 很快 | 简单操作（计数器、标志） |
| 自旋锁 | 快（短临界区） | 锁持有时间 < 1μs |
| 互斥锁 | 较慢 | 锁持有时间 > 1μs |
| 读写锁 | 视场景 | 读多写少 |

## 内存模型

### 为什么需要内存模型？

现代 CPU 和编译器会进行各种优化，这可能改变程序的执行顺序：

```c
// 源代码
int data = 0;
int ready = 0;

// 线程1（生产者）
void producer() {
    data = 42;      // (1)
    ready = 1;      // (2)
}

// 线程2（消费者）
void consumer() {
    if (ready) {    // (3)
        use(data);  // (4)
    }
}

// 问题：编译器或 CPU 可能重排序 (1) 和 (2)
// 结果：线程2 可能看到 ready=1 但 data=0
```

**内存重排序的原因：**

1. **编译器优化**：重新安排指令顺序以提高性能
2. **CPU 乱序执行**：现代 CPU 会并行执行多条指令
3. **缓存一致性**：不同核心的缓存更新不是瞬时的
4. **Store Buffer**：写操作可能被缓冲延迟执行

```
CPU 架构的内存层次：

核心1:                     核心2:
┌────────────┐            ┌────────────┐
│   寄存器   │            │   寄存器   │
├────────────┤            ├────────────┤
│  L1 Cache  │            │  L1 Cache  │
├────────────┤            ├────────────┤
│  L2 Cache  │            │  L2 Cache  │
└────────────┘            └────────────┘
      │                         │
      └──────────┬──────────────┘
                 │
           ┌────────────┐
           │  L3 Cache  │
           │  (共享)    │
           ├────────────┤
           │  主内存    │
           └────────────┘

问题：核心1 写入的数据可能还在 L1 Cache 中，
      核心2 读取时看不到最新值
```

### C11 内存序（Memory Order）

C11 定义了 6 种内存序，用于控制原子操作的同步行为：

```c
typedef enum memory_order {
    memory_order_relaxed,  // 最弱，只保证原子性
    memory_order_consume,  // 很少用，依赖顺序
    memory_order_acquire,  // 获取语义
    memory_order_release,  // 释放语义
    memory_order_acq_rel,  // 获取+释放
    memory_order_seq_cst   // 最强，顺序一致性（默认）
} memory_order;
```

#### 1. memory_order_relaxed - 松弛序

**特性：**
- 只保证操作的原子性
- 不保证任何顺序
- 最快，但最难正确使用
- 适用场景：简单计数器（不依赖顺序）

```c
#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>

atomic_int relaxed_counter = 0;

void *increment_relaxed(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        // 只保证原子性，不保证顺序
        atomic_fetch_add_explicit(&relaxed_counter, 1, memory_order_relaxed);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    
    pthread_create(&t1, NULL, increment_relaxed, NULL);
    pthread_create(&t2, NULL, increment_relaxed, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    // 最终值是正确的，但中间状态可能不一致
    printf("Counter: %d\n", atomic_load(&relaxed_counter));
    return 0;
}
```

#### 2. memory_order_acquire / memory_order_release - 获取/释放语义

**特性：**
- **Release**：写操作之前的所有内存操作不能重排到之后
- **Acquire**：读操作之后的所有内存操作不能重排到之前
- 配对使用形成同步点
- 适用场景：生产者-消费者模式

```c
#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>
#include <assert.h>

// 共享数据
int shared_data = 0;
atomic_bool data_ready = ATOMIC_VAR_INIT(false);

// 生产者
void *producer_acq_rel(void *arg) {
    // 准备数据
    shared_data = 42;
    
    // Release: 确保 shared_data 的写入在 store 之前完成
    atomic_store_explicit(&data_ready, true, memory_order_release);
    
    printf("生产者: 数据已准备\n");
    return NULL;
}

// 消费者
void *consumer_acq_rel(void *arg) {
    // Acquire: 确保 load 之后的读取能看到 store 之前的写入
    while (!atomic_load_explicit(&data_ready, memory_order_acquire)) {
        // 等待
    }
    
    printf("消费者: 读取数据 = %d\n", shared_data);
    assert(shared_data == 42);  // 保证能看到正确的值
    return NULL;
}

int main() {
    pthread_t prod, cons;
    
    pthread_create(&cons, NULL, consumer_acq_rel, NULL);
    pthread_create(&prod, NULL, producer_acq_rel, NULL);
    
    pthread_join(cons, NULL);
    pthread_join(prod, NULL);
    
    return 0;
}
```

**Acquire-Release 的内存屏障效果：**

```
生产者线程:                消费者线程:

data = 42;     ─┐          ┌─ while (!ready);
                │          │
ready = 1;  ────┼─────────→│    (同步点)
(release)       │          │  (acquire)
                │          │
                └──────────┼─> use(data);
                           │   保证能看到 data = 42
```

#### 3. memory_order_seq_cst - 顺序一致性

**特性：**
- 最强的内存序
- 所有线程看到相同的全局操作顺序
- 默认的内存序
- 最慢但最容易理解

```c
#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>

atomic_int x = 0, y = 0;
atomic_int r1 = 0, r2 = 0;

void *thread1_seq_cst(void *arg) {
    atomic_store(&x, 1);  // 默认 memory_order_seq_cst
    r1 = atomic_load(&y);
    return NULL;
}

void *thread2_seq_cst(void *arg) {
    atomic_store(&y, 1);  // 默认 memory_order_seq_cst
    r2 = atomic_load(&x);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    
    for (int i = 0; i < 100000; i++) {
        x = 0; y = 0; r1 = 0; r2 = 0;
        
        pthread_create(&t1, NULL, thread1_seq_cst, NULL);
        pthread_create(&t2, NULL, thread2_seq_cst, NULL);
        
        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
        
        // 使用 seq_cst，不可能 r1 == 0 && r2 == 0
        if (r1 == 0 && r2 == 0) {
            printf("异常情况！r1=%d, r2=%d\n", r1, r2);
        }
    }
    
    printf("测试完成，未发现异常\n");
    return 0;
}
```

#### 内存序对比表

| 内存序 | 保证 | 性能 | 适用场景 |
|--------|------|------|----------|
| **relaxed** | 只保证原子性 | 最快 | 简单计数器 |
| **acquire** | 之后的读不会前移 | 快 | 读取共享状态 |
| **release** | 之前的写不会后移 | 快 | 发布共享状态 |
| **acq_rel** | acquire + release | 中等 | 读-改-写操作 |
| **seq_cst** | 全局顺序 | 最慢 | 默认选择，最安全 |

### 实际案例：无锁环形缓冲区

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

typedef struct {
    int data[BUFFER_SIZE];
    atomic_size_t write_pos;
    atomic_size_t read_pos;
} RingBuffer;

void ring_buffer_init(RingBuffer *rb) {
    atomic_init(&rb->write_pos, 0);
    atomic_init(&rb->read_pos, 0);
}

bool ring_buffer_push(RingBuffer *rb, int value) {
    size_t current_write = atomic_load_explicit(&rb->write_pos, memory_order_relaxed);
    size_t next_write = (current_write + 1) % BUFFER_SIZE;
    size_t current_read = atomic_load_explicit(&rb->read_pos, memory_order_acquire);
    
    if (next_write == current_read) {
        return false;  // 缓冲区满
    }
    
    rb->data[current_write] = value;
    
    // Release: 确保数据写入在更新 write_pos 之前完成
    atomic_store_explicit(&rb->write_pos, next_write, memory_order_release);
    return true;
}

bool ring_buffer_pop(RingBuffer *rb, int *value) {
    size_t current_read = atomic_load_explicit(&rb->read_pos, memory_order_relaxed);
    size_t current_write = atomic_load_explicit(&rb->write_pos, memory_order_acquire);
    
    if (current_read == current_write) {
        return false;  // 缓冲区空
    }
    
    *value = rb->data[current_read];
    size_t next_read = (current_read + 1) % BUFFER_SIZE;
    
    // Release: 确保数据读取在更新 read_pos 之前完成
    atomic_store_explicit(&rb->read_pos, next_read, memory_order_release);
    return true;
}

// 测试
void *producer_ring(void *arg) {
    RingBuffer *rb = arg;
    for (int i = 0; i < 10000; i++) {
        while (!ring_buffer_push(rb, i)) {
            // 等待空位
        }
    }
    return NULL;
}

void *consumer_ring(void *arg) {
    RingBuffer *rb = arg;
    int value;
    for (int i = 0; i < 10000; i++) {
        while (!ring_buffer_pop(rb, &value)) {
            // 等待数据
        }
        if (value != i) {
            printf("错误：期望 %d，实际 %d\n", i, value);
        }
    }
    return NULL;
}

int main() {
    RingBuffer rb;
    ring_buffer_init(&rb);
    
    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer_ring, &rb);
    pthread_create(&cons, NULL, consumer_ring, &rb);
    
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    
    printf("环形缓冲区测试完成\n");
    return 0;
}
```

### 内存屏障（Memory Fence）

显式插入内存屏障，强制同步：

```c
#include <stdatomic.h>

// C11 内存屏障
atomic_thread_fence(memory_order_acquire);
atomic_thread_fence(memory_order_release);
atomic_thread_fence(memory_order_acq_rel);
atomic_thread_fence(memory_order_seq_cst);

// GCC 内存屏障
__atomic_thread_fence(__ATOMIC_ACQUIRE);
__atomic_thread_fence(__ATOMIC_RELEASE);
__atomic_thread_fence(__ATOMIC_SEQ_CST);

// 旧版 GCC
__sync_synchronize();  // 完全屏障

// 特定架构的屏障
#if defined(__x86_64__) || defined(__i386__)
    __asm__ __volatile__("mfence" ::: "memory");  // 完全屏障
    __asm__ __volatile__("lfence" ::: "memory");  // 读屏障
    __asm__ __volatile__("sfence" ::: "memory");  // 写屏障
#elif defined(__aarch64__)
    __asm__ __volatile__("dmb ish" ::: "memory");  // 数据内存屏障
    __asm__ __volatile__("dsb ish" ::: "memory");  // 数据同步屏障
#endif
```

### 内存模型的最佳实践

```c
// ✅ 推荐：默认使用 seq_cst
atomic_int x = 0;
atomic_store(&x, 42);  // 隐式 memory_order_seq_cst

// ✅ 推荐：生产者-消费者使用 acquire-release
atomic_store_explicit(&ready, 1, memory_order_release);
while (!atomic_load_explicit(&ready, memory_order_acquire));

// ⚠️ 小心：只在确定安全时使用 relaxed
atomic_fetch_add_explicit(&counter, 1, memory_order_relaxed);

// ❌ 避免：不要在复杂逻辑中使用 relaxed
// 除非你完全理解内存模型并做过充分测试
```

## 竞态条件与死锁

### 竞态条件（Race Condition）

**定义：**多个线程同时访问共享数据，且至少有一个线程进行写操作，而没有适当的同步机制。

#### 经典案例1：银行账户

```c
#include <stdio.h>
#include <pthread.h>

typedef struct {
    int balance;
    pthread_mutex_t mutex;
} BankAccount;

// ❌ 错误：存在竞态条件
void withdraw_unsafe(BankAccount *account, int amount) {
    if (account->balance >= amount) {
        // 问题：这里可能被打断！
        account->balance -= amount;
    }
}

// 场景：
// 线程1: 检查余额 (balance=100, amount=80) ✓
// [被打断]
// 线程2: 检查余额 (balance=100, amount=80) ✓
// 线程2: 扣款 (balance=20)
// 线程1: 恢复，扣款 (balance=-60) ← 透支了！

// ✅ 正确：使用互斥锁
void withdraw_safe(BankAccount *account, int amount) {
    pthread_mutex_lock(&account->mutex);
    
    if (account->balance >= amount) {
        account->balance -= amount;
        printf("取款 %d，余额 %d\n", amount, account->balance);
    } else {
        printf("余额不足\n");
    }
    
    pthread_mutex_unlock(&account->mutex);
}

int main() {
    BankAccount account = {
        .balance = 100,
        .mutex = PTHREAD_MUTEX_INITIALIZER
    };
    
    // 测试并发取款
    pthread_t threads[2];
    int amounts[] = {80, 80};
    
    for (int i = 0; i < 2; i++) {
        pthread_create(&threads[i], NULL, 
                      (void *(*)(void *))withdraw_safe, &account);
    }
    
    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("最终余额: %d\n", account.balance);
    
    pthread_mutex_destroy(&account->mutex);
    return 0;
}
```

#### 经典案例2：检查-使用模式（Check-Then-Act）

```c
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct {
    bool is_initialized;
    int data;
    pthread_mutex_t mutex;
} Resource;

// ❌ 错误：TOCTOU（Time-Of-Check-Time-Of-Use）漏洞
void init_unsafe(Resource *res) {
    if (!res->is_initialized) {  // (1) 检查
        // 可能被打断！
        res->data = 42;          // (2) 使用
        res->is_initialized = true;
    }
}

// ✅ 正确：原子化检查和操作
void init_safe(Resource *res) {
    pthread_mutex_lock(&res->mutex);
    
    if (!res->is_initialized) {
        res->data = 42;
        res->is_initialized = true;
    }
    
    pthread_mutex_unlock(&res->mutex);
}

// ✅ 更好：双重检查锁定（Double-Checked Locking）
void init_dcl(Resource *res) {
    // 快速路径：已初始化则直接返回
    if (res->is_initialized) {
        return;
    }
    
    pthread_mutex_lock(&res->mutex);
    
    // 再次检查（可能其他线程已经初始化了）
    if (!res->is_initialized) {
        res->data = 42;
        res->is_initialized = true;
    }
    
    pthread_mutex_unlock(&res->mutex);
}
```

#### 经典案例3：非原子的读-改-写

```c
#include <stdio.h>
#include <pthread.h>

typedef struct {
    int counter;
    pthread_mutex_t mutex;
} Counter;

// ❌ 错误：递增操作不是原子的
void increment_unsafe(Counter *cnt) {
    cnt->counter++;
    // 实际上是：
    // int temp = cnt->counter;  // 读
    // temp = temp + 1;          // 改
    // cnt->counter = temp;      // 写
}

// ✅ 正确：使用互斥锁
void increment_safe_mutex(Counter *cnt) {
    pthread_mutex_lock(&cnt->mutex);
    cnt->counter++;
    pthread_mutex_unlock(&cnt->mutex);
}

// ✅ 更好：使用原子操作
#include <stdatomic.h>
atomic_int atomic_counter = 0;

void increment_safe_atomic() {
    atomic_fetch_add(&atomic_counter, 1);
}
```

#### 检测竞态条件的工具

**1. ThreadSanitizer (TSan)**

```bash
# 编译时启用 TSan
gcc -fsanitize=thread -g -O1 program.c -o program -lpthread

# 运行
./program

# TSan 会报告数据竞争：
# ==================
# WARNING: ThreadSanitizer: data race (pid=12345)
#   Write of size 4 at 0x7b0000000000 by thread T2:
#     #0 increment_unsafe program.c:25
#   Previous write of size 4 at 0x7b0000000000 by thread T1:
#     #0 increment_unsafe program.c:25
# ==================
```

**2. Valgrind Helgrind**

```bash
# 编译（带调试信息）
gcc -g program.c -o program -lpthread

# 使用 Helgrind 检测
valgrind --tool=helgrind ./program

# 输出示例：
# ==12345== Possible data race during write of size 4 at 0x10000
# ==12345==    at 0x12345: increment_unsafe (program.c:25)
# ==12345==  This conflicts with a previous write of size 4 by thread #2
```

**3. 添加断言和日志**

```c
#include <stdio.h>
#include <pthread.h>
#include <assert.h>

typedef struct {
    int value;
    pthread_t owner;  // 追踪当前所有者
    pthread_mutex_t mutex;
} DebugData;

void debug_write(DebugData *data, int new_value) {
    pthread_mutex_lock(&data->mutex);
    
    // 记录所有者
    data->owner = pthread_self();
    
    printf("[线程 %lu] 写入 %d\n", pthread_self(), new_value);
    data->value = new_value;
    
    pthread_mutex_unlock(&data->mutex);
}

void debug_read(DebugData *data) {
    pthread_mutex_lock(&data->mutex);
    
    printf("[线程 %lu] 读取 %d (所有者: %lu)\n", 
           pthread_self(), data->value, data->owner);
    
    pthread_mutex_unlock(&data->mutex);
}
```

### 死锁（Deadlock）

**定义：**两个或多个线程互相等待对方持有的资源，导致所有线程都无法继续执行。

#### 死锁的四个必要条件

1. **互斥（Mutual Exclusion）**：资源不能被多个线程同时使用
2. **持有并等待（Hold and Wait）**：线程持有资源的同时等待其他资源
3. **不可抢占（No Preemption）**：资源只能由持有者主动释放
4. **循环等待（Circular Wait）**：存在资源等待的环形链

```
死锁场景示意图：

线程1持有A，等待B    线程2持有B，等待A
     ↓                     ↓
   [ A ] ←───────────────[ 线程1 ]
     ↑                      │
     │                      │ 等待
     │                      ↓
     └────── 等待 ────── [ B ]
                            ↑
                            │
                       [ 线程2 ]
```

#### 经典案例：哲学家就餐问题

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define N_PHILOSOPHERS 5

pthread_mutex_t forks[N_PHILOSOPHERS];

// ❌ 错误：会导致死锁
void *philosopher_deadlock(void *arg) {
    int id = *(int *)arg;
    int left_fork = id;
    int right_fork = (id + 1) % N_PHILOSOPHERS;
    
    while (1) {
        // 思考
        printf("哲学家 %d 正在思考...\n", id);
        sleep(1);
        
        // 拿起左边的叉子
        pthread_mutex_lock(&forks[left_fork]);
        printf("哲学家 %d 拿起了左边的叉子\n", id);
        
        // 如果所有哲学家同时拿起左叉子，就会死锁！
        
        // 拿起右边的叉子
        pthread_mutex_lock(&forks[right_fork]);
        printf("哲学家 %d 拿起了右边的叉子\n", id);
        
        // 吃饭
        printf("哲学家 %d 正在吃饭\n", id);
        sleep(1);
        
        // 放下叉子
        pthread_mutex_unlock(&forks[right_fork]);
        pthread_mutex_unlock(&forks[left_fork]);
    }
    
    return NULL;
}

// ✅ 解决方案1：资源排序（打破循环等待）
void *philosopher_ordered(void *arg) {
    int id = *(int *)arg;
    int left_fork = id;
    int right_fork = (id + 1) % N_PHILOSOPHERS;
    
    // 总是先获取编号小的叉子
    int first_fork = left_fork < right_fork ? left_fork : right_fork;
    int second_fork = left_fork < right_fork ? right_fork : left_fork;
    
    while (1) {
        printf("哲学家 %d 正在思考...\n", id);
        sleep(1);
        
        pthread_mutex_lock(&forks[first_fork]);
        pthread_mutex_lock(&forks[second_fork]);
        
        printf("哲学家 %d 正在吃饭\n", id);
        sleep(1);
        
        pthread_mutex_unlock(&forks[second_fork]);
        pthread_mutex_unlock(&forks[first_fork]);
    }
    
    return NULL;
}

// ✅ 解决方案2：使用 trylock（打破持有并等待）
void *philosopher_trylock(void *arg) {
    int id = *(int *)arg;
    int left_fork = id;
    int right_fork = (id + 1) % N_PHILOSOPHERS;
    
    while (1) {
        printf("哲学家 %d 正在思考...\n", id);
        sleep(1);
        
        pthread_mutex_lock(&forks[left_fork]);
        
        // 尝试获取右叉子，如果失败则释放左叉子
        if (pthread_mutex_trylock(&forks[right_fork]) != 0) {
            pthread_mutex_unlock(&forks[left_fork]);
            continue;  // 重试
        }
        
        printf("哲学家 %d 正在吃饭\n", id);
        sleep(1);
        
        pthread_mutex_unlock(&forks[right_fork]);
        pthread_mutex_unlock(&forks[left_fork]);
    }
    
    return NULL;
}

// ✅ 解决方案3：限制并发数（打破互斥）
#include <semaphore.h>
sem_t room;  // 最多允许 N-1 个哲学家同时就餐

void *philosopher_semaphore(void *arg) {
    int id = *(int *)arg;
    int left_fork = id;
    int right_fork = (id + 1) % N_PHILOSOPHERS;
    
    while (1) {
        printf("哲学家 %d 正在思考...\n", id);
        sleep(1);
        
        sem_wait(&room);  // 进入"餐厅"
        
        pthread_mutex_lock(&forks[left_fork]);
        pthread_mutex_lock(&forks[right_fork]);
        
        printf("哲学家 %d 正在吃饭\n", id);
        sleep(1);
        
        pthread_mutex_unlock(&forks[right_fork]);
        pthread_mutex_unlock(&forks[left_fork]);
        
        sem_post(&room);  // 离开"餐厅"
    }
    
    return NULL;
}

int main() {
    pthread_t philosophers[N_PHILOSOPHERS];
    int ids[N_PHILOSOPHERS];
    
    // 初始化叉子（互斥锁）
    for (int i = 0; i < N_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks[i], NULL);
        ids[i] = i;
    }
    
    // 初始化信号量（用于解决方案3）
    sem_init(&room, 0, N_PHILOSOPHERS - 1);
    
    // 创建哲学家线程
    for (int i = 0; i < N_PHILOSOPHERS; i++) {
        pthread_create(&philosophers[i], NULL, 
                      philosopher_ordered,  // 使用有序方案
                      &ids[i]);
    }
    
    // 等待（实际上会一直运行）
    for (int i = 0; i < N_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }
    
    return 0;
}
```

#### 避免死锁的策略

**1. 锁排序（Lock Ordering）**

```c
// 给所有锁分配全局顺序，总是按顺序获取
pthread_mutex_t lock_a, lock_b;

void safe_function() {
    // 总是先获取 lock_a，再获取 lock_b
    pthread_mutex_lock(&lock_a);
    pthread_mutex_lock(&lock_b);
    
    // 临界区
    
    pthread_mutex_unlock(&lock_b);
    pthread_mutex_unlock(&lock_a);
}
```

**2. 一次性获取所有锁**

```c
void acquire_all_locks(pthread_mutex_t *locks, int count) {
    // 原子地获取所有锁（实际需要额外的同步机制）
    pthread_mutex_lock(&global_meta_lock);
    
    for (int i = 0; i < count; i++) {
        pthread_mutex_lock(&locks[i]);
    }
    
    pthread_mutex_unlock(&global_meta_lock);
}
```

**3. 使用超时**

```c
#include <time.h>

bool try_lock_with_timeout(pthread_mutex_t *lock, int timeout_ms) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    
    int result = pthread_mutex_timedlock(lock, &ts);
    return result == 0;
}

void safe_with_timeout() {
    if (!try_lock_with_timeout(&lock_a, 1000)) {
        // 超时，可能死锁，放弃
        return;
    }
    
    if (!try_lock_with_timeout(&lock_b, 1000)) {
        pthread_mutex_unlock(&lock_a);
        return;
    }
    
    // 成功获取所有锁
    // ...
    
    pthread_mutex_unlock(&lock_b);
    pthread_mutex_unlock(&lock_a);
}
```

**4. 死锁检测和恢复**

```c
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct {
    pthread_mutex_t *lock;
    pthread_t holder;
    bool is_locked;
} LockInfo;

LockInfo lock_registry[100];
int num_locks = 0;

// 记录锁的获取
void register_lock_acquisition(pthread_mutex_t *lock) {
    for (int i = 0; i < num_locks; i++) {
        if (lock_registry[i].lock == lock) {
            lock_registry[i].holder = pthread_self();
            lock_registry[i].is_locked = true;
            return;
        }
    }
}

// 检测循环等待
bool detect_deadlock() {
    // 简化的死锁检测算法
    // 实际应该构建等待图并检测环
    return false;
}
```

#### 检测死锁的工具

```bash
# 1. ThreadSanitizer 可以检测死锁
gcc -fsanitize=thread program.c -o program -lpthread

# 2. Valgrind DRD
valgrind --tool=drd ./program

# 3. pstack 查看线程堆栈
# 在死锁发生时：
pstack <pid>

# 4. GDB 调试
gdb -p <pid>
(gdb) info threads     # 查看所有线程
(gdb) thread 2         # 切换到线程2
(gdb) bt               # 查看调用栈
```

### 活锁（Livelock）

活锁是指线程不断改变状态但无法继续执行：

```c
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

pthread_mutex_t lock1, lock2;

// ❌ 活锁示例：两个线程都很"礼貌"
void *polite_thread1(void *arg) {
    while (1) {
        pthread_mutex_lock(&lock1);
        
        if (pthread_mutex_trylock(&lock2) != 0) {
            // "礼貌"地释放已获取的锁
            pthread_mutex_unlock(&lock1);
            // 但立即重试，可能与线程2同步，形成活锁
            continue;
        }
        
        // 工作
        printf("线程1 工作中\n");
        
        pthread_mutex_unlock(&lock2);
        pthread_mutex_unlock(&lock1);
        break;
    }
    return NULL;
}

void *polite_thread2(void *arg) {
    while (1) {
        pthread_mutex_lock(&lock2);
        
        if (pthread_mutex_trylock(&lock1) != 0) {
            pthread_mutex_unlock(&lock2);
            continue;  // 与线程1可能形成活锁
        }
        
        printf("线程2 工作中\n");
        
        pthread_mutex_unlock(&lock1);
        pthread_mutex_unlock(&lock2);
        break;
    }
    return NULL;
}

// ✅ 解决：添加随机延迟，打破同步
void *smart_thread1(void *arg) {
    while (1) {
        pthread_mutex_lock(&lock1);
        
        if (pthread_mutex_trylock(&lock2) != 0) {
            pthread_mutex_unlock(&lock1);
            usleep(rand() % 1000);  // 随机延迟
            continue;
        }
        
        printf("线程1 工作中\n");
        
        pthread_mutex_unlock(&lock2);
        pthread_mutex_unlock(&lock1);
        break;
    }
    return NULL;
}
```

### 饥饿（Starvation）

某些线程永远无法获得所需资源：

```c
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t resource;

// ❌ 可能导致饥饿
void *high_priority_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&resource);
        // 长时间持有锁
        usleep(100000);
        pthread_mutex_unlock(&resource);
        // 立即再次请求
    }
}

void *low_priority_thread(void *arg) {
    // 可能永远无法获得锁
    pthread_mutex_lock(&resource);
    printf("低优先级线程获得了资源\n");
    pthread_mutex_unlock(&resource);
    return NULL;
}

// ✅ 解决：公平锁或使用信号量限制
void fair_resource_access() {
    // 使用 PTHREAD_PRIO_INHERIT 或公平锁
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
    pthread_mutex_init(&resource, &attr);
}
```

## 线程安全设计

### 什么是线程安全？

**线程安全**的函数或数据结构可以被多个线程同时调用而不会导致数据竞争或不一致状态。

**线程安全级别：**

| 级别 | 描述 | 示例 |
|------|------|------|
| **不可变（Immutable）** | 对象创建后不能修改 | 字符串常量 |
| **线程安全（Thread-Safe）** | 可被多线程安全调用 | `strtok_r()` |
| **条件线程安全** | 需要外部同步 | `printf()` |
| **线程不安全** | 不能多线程调用 | `strtok()`, `rand()` |

### 设计模式

#### 1. 不可变对象

```c
// 不可变字符串
typedef struct {
    const char *data;
    size_t length;
} ImmutableString;

ImmutableString *string_create(const char *str) {
    ImmutableString *s = malloc(sizeof(ImmutableString));
    s->length = strlen(str);
    char *copy = malloc(s->length + 1);
    strcpy(copy, str);
    s->data = copy;
    return s;  // 不可变，天然线程安全
}
```

#### 2. 线程局部存储

```c
// 每个线程独立的错误码
__thread int thread_errno = 0;

void set_error(int code) {
    thread_errno = code;  // 线程安全
}
```

#### 3. 读写分离

```c
// 使用 RCU (Read-Copy-Update) 模式
typedef struct Config {
    char *server_url;
    int timeout;
} Config;

Config *global_config = NULL;
pthread_rwlock_t config_lock = PTHREAD_RWLOCK_INITIALIZER;

// 读取配置（无锁）
Config *get_config() {
    Config *cfg;
    pthread_rwlock_rdlock(&config_lock);
    cfg = global_config;
    pthread_rwlock_unlock(&config_lock);
    return cfg;
}

// 更新配置（写时复制）
void update_config(const char *url, int timeout) {
    Config *new_cfg = malloc(sizeof(Config));
    new_cfg->server_url = strdup(url);
    new_cfg->timeout = timeout;
    
    pthread_rwlock_wrlock(&config_lock);
    Config *old_cfg = global_config;
    global_config = new_cfg;
    pthread_rwlock_unlock(&config_lock);
    
    // 延迟释放旧配置
    free(old_cfg->server_url);
    free(old_cfg);
}
```

### 常见陷阱

```c
// ❌ 陷阱1：返回指向栈变量的指针
void *bad_thread(void *arg) {
    int local = 42;
    return &local;  // 危险！
}

// ❌ 陷阱2：在锁内执行耗时操作
void bad_design() {
    pthread_mutex_lock(&mutex);
    expensive_computation();  // 阻塞其他线程
    pthread_mutex_unlock(&mutex);
}

// ❌ 陷阱3：忘记初始化同步原语
pthread_mutex_t mutex;  // 未初始化！
pthread_mutex_lock(&mutex);  // 未定义行为
```

## 线程池实现

线程池是一种预先创建一组工作线程,然后重复使用它们执行任务的模式,避免频繁创建/销毁线程的开销。

### 完整的线程池实现

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

// 任务结构
typedef struct Task {
    void (*function)(void *arg);
    void *argument;
    struct Task *next;
} Task;

// 任务队列
typedef struct {
    Task *head;
    Task *tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
} TaskQueue;

// 线程池
typedef struct {
    pthread_t *threads;
    int thread_count;
    TaskQueue queue;
    bool shutdown;
} ThreadPool;

// 初始化任务队列
void task_queue_init(TaskQueue *q) {
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}

// 添加任务
void task_queue_push(TaskQueue *q, Task *task) {
    pthread_mutex_lock(&q->mutex);
    
    task->next = NULL;
    if (q->tail) {
        q->tail->next = task;
    } else {
        q->head = task;
    }
    q->tail = task;
    q->count++;
    
    pthread_cond_signal(&q->not_empty);  // 唤醒等待的工作线程
    pthread_mutex_unlock(&q->mutex);
}

// 取出任务
Task *task_queue_pop(TaskQueue *q) {
    pthread_mutex_lock(&q->mutex);
    
    while (q->head == NULL) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    
    Task *task = q->head;
    q->head = task->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    q->count--;
    
    pthread_mutex_unlock(&q->mutex);
    return task;
}

// 工作线程函数
void *worker_thread(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;
    
    printf("工作线程 %lu 启动\n", pthread_self());
    
    while (!pool->shutdown) {
        Task *task = task_queue_pop(&pool->queue);
        
        if (task) {
            printf("线程 %lu 执行任务\n", pthread_self());
            task->function(task->argument);
            free(task);
        }
    }
    
    printf("工作线程 %lu 退出\n", pthread_self());
    return NULL;
}

// 创建线程池
ThreadPool *thread_pool_create(int num_threads) {
    ThreadPool *pool = malloc(sizeof(ThreadPool));
    
    pool->thread_count = num_threads;
    pool->threads = malloc(sizeof(pthread_t) * num_threads);
    pool->shutdown = false;
    
    task_queue_init(&pool->queue);
    
    // 创建工作线程
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&pool->threads[i], NULL, worker_thread, pool);
    }
    
    return pool;
}

// 提交任务
void thread_pool_submit(ThreadPool *pool, void (*function)(void *), void *arg) {
    Task *task = malloc(sizeof(Task));
    task->function = function;
    task->argument = arg;
    task_queue_push(&pool->queue, task);
}

// 销毁线程池
void thread_pool_destroy(ThreadPool *pool) {
    pool->shutdown = true;
    
    // 唤醒所有工作线程
    pthread_cond_broadcast(&pool->queue.not_empty);
    
    // 等待所有线程结束
    for (int i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    
    // 清理资源
    free(pool->threads);
    pthread_mutex_destroy(&pool->queue.mutex);
    pthread_cond_destroy(&pool->queue.not_empty);
    free(pool);
}

// 示例任务
void example_task(void *arg) {
    int id = *(int *)arg;
    printf("  任务 %d 开始执行\n", id);
    sleep(1);  // 模拟工作
    printf("  任务 %d 完成\n", id);
    free(arg);
}

int main() {
    // 创建有 4 个工作线程的线程池
    ThreadPool *pool = thread_pool_create(4);
    
    // 提交 10 个任务
    for (int i = 0; i < 10; i++) {
        int *arg = malloc(sizeof(int));
        *arg = i;
        thread_pool_submit(pool, example_task, arg);
    }
    
    // 等待所有任务完成
    sleep(5);
    
    // 销毁线程池
    thread_pool_destroy(pool);
    
    return 0;
}
```

## 生产者-消费者模型

经典的并发模式,生产者生产数据放入缓冲区,消费者从缓冲区取出数据处理。

### 有界缓冲区实现

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 10

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;   // 写入位置
    int out;  // 读取位置
    
    sem_t empty;  // 空槽位数量
    sem_t full;   // 满槽位数量
    pthread_mutex_t mutex;
} BoundedBuffer;

void bounded_buffer_init(BoundedBuffer *bb) {
    bb->in = 0;
    bb->out = 0;
    sem_init(&bb->empty, 0, BUFFER_SIZE);
    sem_init(&bb->full, 0, 0);
    pthread_mutex_init(&bb->mutex, NULL);
}

void bounded_buffer_put(BoundedBuffer *bb, int item) {
    sem_wait(&bb->empty);  // 等待空槽位
    pthread_mutex_lock(&bb->mutex);
    
    bb->buffer[bb->in] = item;
    printf("生产: %d 到位置 %d\n", item, bb->in);
    bb->in = (bb->in + 1) % BUFFER_SIZE;
    
    pthread_mutex_unlock(&bb->mutex);
    sem_post(&bb->full);  // 增加满槽位
}

int bounded_buffer_get(BoundedBuffer *bb) {
    sem_wait(&bb->full);  // 等待满槽位
    pthread_mutex_lock(&bb->mutex);
    
    int item = bb->buffer[bb->out];
    printf("  消费: %d 从位置 %d\n", item, bb->out);
    bb->out = (bb->out + 1) % BUFFER_SIZE;
    
    pthread_mutex_unlock(&bb->mutex);
    sem_post(&bb->empty);  // 增加空槽位
    
    return item;
}

void *producer_func(void *arg) {
    BoundedBuffer *bb = arg;
    for (int i = 0; i < 20; i++) {
        bounded_buffer_put(bb, i);
        usleep(100000);  // 100ms
    }
    return NULL;
}

void *consumer_func(void *arg) {
    BoundedBuffer *bb = arg;
    for (int i = 0; i < 20; i++) {
        int item = bounded_buffer_get(bb);
        usleep(150000);  // 150ms
    }
    return NULL;
}

int main() {
    BoundedBuffer bb;
    bounded_buffer_init(&bb);
    
    pthread_t producer, consumer;
    pthread_create(&producer, NULL, producer_func, &bb);
    pthread_create(&consumer, NULL, consumer_func, &bb);
    
    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);
    
    sem_destroy(&bb.empty);
    sem_destroy(&bb.full);
    pthread_mutex_destroy(&bb.mutex);
    
    return 0;
}
```

## 调试并发问题

### 使用 ThreadSanitizer

```bash
# 编译
gcc -fsanitize=thread -g -O1 program.c -o program -lpthread

# 运行
./program

# 设置选项
export TSAN_OPTIONS="halt_on_error=1:second_deadlock_stack=1"
./program
```

### 使用 Valgrind Helgrind

```bash
# 检测数据竞争
valgrind --tool=helgrind --log-file=helgrind.log ./program

# 检测死锁
valgrind --tool=drd --check-stack-var=yes ./program
```

### 添加调试日志

```c
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define DEBUG_LOG(fmt, ...) do { \
    struct timespec ts; \
    clock_gettime(CLOCK_REALTIME, &ts); \
    printf("[%lu.%09ld][线程 %lu] " fmt "\n", \
           ts.tv_sec, ts.tv_nsec, pthread_self(), ##__VA_ARGS__); \
} while(0)

void *debug_thread(void *arg) {
    DEBUG_LOG("线程启动");
    sleep(1);
    DEBUG_LOG("线程完成工作");
    return NULL;
}
```

### 压力测试

```c
#include <stdio.h>
#include <pthread.h>
#include <assert.h>

#define NUM_THREADS 100
#define ITERATIONS 100000

atomic_int counter = 0;

void *stress_test(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        atomic_fetch_add(&counter, 1);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, stress_test, NULL);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    int expected = NUM_THREADS * ITERATIONS;
    assert(counter == expected);
    printf("压力测试通过: %d == %d\n", counter, expected);
    
    return 0;
}
```

## 性能优化技巧

### 1. 减少锁粒度

```c
// ❌ 粗粒度
pthread_mutex_lock(&global_lock);
process_all_data();
pthread_mutex_unlock(&global_lock);

// ✅ 细粒度
for (int i = 0; i < n; i++) {
    pthread_mutex_lock(&locks[i]);
    process_data(i);
    pthread_mutex_unlock(&locks[i]);
}
```

### 2. 使用读写锁

```c
// 读多写少场景
pthread_rwlock_t rwlock;

void reader() {
    pthread_rwlock_rdlock(&rwlock);
    read_data();
    pthread_rwlock_unlock(&rwlock);
}

void writer() {
    pthread_rwlock_wrlock(&rwlock);
    write_data();
    pthread_rwlock_unlock(&rwlock);
}
```

### 3. 无锁数据结构

```c
// 使用原子操作避免锁
atomic_int shared_value;
atomic_fetch_add(&shared_value, 1);
```

### 4. 批量处理

```c
// ❌ 每次一个
for (int i = 0; i < 1000; i++) {
    pthread_mutex_lock(&mutex);
    queue_push(i);
    pthread_mutex_unlock(&mutex);
}

// ✅ 批量处理
pthread_mutex_lock(&mutex);
for (int i = 0; i < 1000; i++) {
    queue_push(i);
}
pthread_mutex_unlock(&mutex);
```

## 其他并发模型

### 事件驱动（Event-Driven）

```c
// 使用 libevent 示例
#include <event2/event.h>

void callback(evutil_socket_t fd, short events, void *arg) {
    // 处理事件
}

int main() {
    struct event_base *base = event_base_new();
    struct event *ev = event_new(base, fd, EV_READ|EV_PERSIST, callback, NULL);
    event_add(ev, NULL);
    event_base_dispatch(base);
    return 0;
}
```

### 协程（Coroutines）

```c
// 用户态轻量级线程
// 可以使用 libco, libtask 等库实现
```

## 总结

### 并发编程的黄金法则

1. **正确性优先**：先确保程序正确，再考虑性能
2. **简单优先**：优先使用简单的同步机制（互斥锁、信号量）
3. **测试充分**：编写单元测试和压力测试
4. **使用工具**：TSan、Helgrind 等工具检测问题
5. **文档清晰**：记录锁的使用规则和不变量

### 性能优化顺序

1. **测量**：先测量性能瓶颈
2. **算法**：优化算法比优化锁更重要
3. **锁粒度**：减小锁的范围
4. **锁类型**：选择合适的锁（读写锁、自旋锁）
5. **无锁**：最后考虑无锁数据结构

### 常用工具

| 工具 | 用途 | 命令 |
|------|------|------|
| **ThreadSanitizer** | 检测数据竞争 | `gcc -fsanitize=thread` |
| **Helgrind** | 检测数据竞争和死锁 | `valgrind --tool=helgrind` |
| **DRD** | 检测数据竞争 | `valgrind --tool=drd` |
| **gdb** | 调试死锁 | `gdb -p <pid>` |
| **perf** | 性能分析 | `perf record -g` |

### 推荐资源

- **书籍**：
  - 《Unix 网络编程 卷2：进程间通信》
  - 《C++ Concurrency in Action》
  - 《The Art of Multiprocessor Programming》

- **在线资源**：
  - [POSIX Threads Programming](https://computing.llnl.gov/tutorials/pthreads/)
  - [C11 Atomic Operations](https://en.cppreference.com/w/c/atomic)
  - [Linux Kernel Documentation](https://www.kernel.org/doc/html/latest/)

### 最佳实践清单

- ✅ 总是初始化同步原语
- ✅ 总是检查函数返回值
- ✅ 统一加锁顺序避免死锁
- ✅ 在循环中使用 `pthread_cond_wait`
- ✅ 使用 RAII 模式管理资源
- ✅ 编写单元测试覆盖并发场景
- ✅ 使用工具检测数据竞争
- ✅ 记录并发不变量
- ❌ 不要在锁内执行耗时操作
- ❌ 不要忘记释放锁
- ❌ 不要返回栈变量的指针

并发编程是一个复杂的主题，需要不断实践和学习。从简单的互斥锁开始，逐步掌握更高级的技术。记住：**正确性永远比性能更重要**！
