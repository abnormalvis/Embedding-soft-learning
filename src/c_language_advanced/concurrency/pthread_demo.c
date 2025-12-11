#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
struct Task {
    int id;
    int iterations;
};
static void *worker(void *arg) {
    struct Task *task = arg;
    for (int i = 0; i < task->iterations; ++i) {
        printf("thread %d iteration %d\n", task->id, i);
    }
    return NULL;
}
int main(void) {
    pthread_t threads[2];
    struct Task tasks[2] = {
        { .id = 0, .iterations = 3 },
        { .id = 1, .iterations = 2 }
    };
    for (size_t i = 0; i < 2; ++i) {
        if (pthread_create(&threads[i], NULL, worker, &tasks[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }
    for (size_t i = 0; i < 2; ++i) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}
