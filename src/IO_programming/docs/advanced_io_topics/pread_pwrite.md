# pread/pwrite：原子随机读写（不改变文件偏移）

`pread(2)` / `pwrite(2)` 在指定偏移进行读写，但**不会改变 FD 当前偏移量**。它们在多线程/并发场景中非常实用。

## 1. 头文件与函数原型

```c
#include <unistd.h>

ssize_t pread(int fd, void *buf, size_t count, off_t offset);
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);
```

## 2. 为什么说“原子”

这里的“原子”主要指：

- 对同一次调用来说，偏移与读写在内核里作为一个操作完成
- 不需要 `lseek + read/write` 的组合，因此避免线程间偏移互相干扰

并不意味着多次调用不会交错。

## 3. 使用场景

- 多线程读取同一文件不同区间
- 实现文件格式解析（随机访问）
- 实现简单 KV 存储（固定偏移读写）

## 4. 注意事项

- 仍需处理短读短写与 `EINTR`。
- 对非 seekable 的 FD（pipe/socket）通常不适用。

## 5. 练习

写一个程序：启动 N 个线程，对同一文件不同 offset 做 `pread`，比较与 `lseek+read` 的实现复杂度差异。
### 参考实现

```c
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 4
#define BLOCK_SIZE 256

typedef struct {
    int thread_id;
    int fd;
    off_t offset;
    char buffer[BLOCK_SIZE];
} thread_arg_t;

// 方法 1: 使用 pread（推荐，线程安全）
void *thread_pread(void *arg) {
    thread_arg_t *ta = (thread_arg_t *)arg;
    
    printf("Thread %d: reading %d bytes from offset %ld using pread\n",
           ta->thread_id, BLOCK_SIZE, ta->offset);
    
    ssize_t n = pread(ta->fd, ta->buffer, BLOCK_SIZE, ta->offset);
    if (n < 0) {
        perror("pread");
        return NULL;
    }
    
    printf("Thread %d: pread got %ld bytes\n", ta->thread_id, n);
    
    return NULL;
}

// 方法 2: 使用 lseek + read（需要锁保护）
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *thread_lseek_read(void *arg) {
    thread_arg_t *ta = (thread_arg_t *)arg;
    
    printf("Thread %d: reading %d bytes from offset %ld using lseek+read\n",
           ta->thread_id, BLOCK_SIZE, ta->offset);
    
    pthread_mutex_lock(&lock);
    
    // 移动到指定位置
    if (lseek(ta->fd, ta->offset, SEEK_SET) < 0) {
        perror("lseek");
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    
    // 读取数据
    ssize_t n = read(ta->fd, ta->buffer, BLOCK_SIZE);
    if (n < 0) {
        perror("read");
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    
    pthread_mutex_unlock(&lock);
    
    printf("Thread %d: lseek+read got %ld bytes\n", ta->thread_id, n);
    
    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file> [pread|lseek]\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    const char *method = argc > 2 ? argv[2] : "pread";

    // 打开文件
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // 创建线程参数
    thread_arg_t threads[NUM_THREADS];
    pthread_t tids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i].thread_id = i;
        threads[i].fd = fd;
        threads[i].offset = (off_t)(i * BLOCK_SIZE);  // 不同的 offset
        memset(threads[i].buffer, 0, BLOCK_SIZE);
    }

    printf("Using method: %s\n", method);
    printf("========================================\n");

    // 启动线程
    for (int i = 0; i < NUM_THREADS; i++) {
        if (strcmp(method, "lseek") == 0) {
            pthread_create(&tids[i], NULL, thread_lseek_read, &threads[i]);
        } else {
            pthread_create(&tids[i], NULL, thread_pread, &threads[i]);
        }
    }

    // 等待所有线程
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(tids[i], NULL);
    }

    close(fd);

    printf("========================================\n");
    printf("All threads completed.\n");

    return 0;
}
```

编译运行：
```bash
gcc -o pread_test pread_test.c -lpthread
./pread_test /etc/passwd pread       # 使用 pread
./pread_test /etc/passwd lseek       # 使用 lseek+read
```

**关键点对比：**

| 特性 | `pread` | `lseek + read` |
|-----|--------|----------------|
| 原子性 | 原子操作 | 需要加锁保护 |
| 线程安全 | 天然线程安全 | 需要互斥锁 |
| 简洁性 | 简洁，参数清晰 | 复杂，需要管理偏移 |
| 性能 | 无额外开销 | 加锁/解锁开销 |
| 用途 | 多线程随机读取 | 单线程或需要保存偏移 |

**实现复杂度差异：**
- `pread` 方案：没有额外的同步机制，代码更清晰
- `lseek+read` 方案：需要为每次操作加锁，容易出错，性能受锁竞争影响

**建议：** 在多线程环境下优先使用 `pread`/`pwrite`，这样既避免了复杂的同步逻辑，又能获得更好的性能。