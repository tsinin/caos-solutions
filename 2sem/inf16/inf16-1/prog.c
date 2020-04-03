#include <stdio.h>
#include <pthread.h>
#include <malloc.h>
#include <limits.h>

int* thread_func(void* v) {
    int tmp;
    int* sum = malloc(4);
    *sum = 0;
    while (scanf("%d", &tmp) != EOF) {
        *sum += tmp;
    }
    return sum;
}

int main(int argc, char** argv) {
    int THREADS_NUM;
    sscanf(argv[1], "%d", &THREADS_NUM);
    pthread_t threads[THREADS_NUM];

    pthread_attr_t* memory_limit_attr = malloc(sizeof(pthread_attr_t));
    pthread_attr_init(memory_limit_attr);
    pthread_attr_setstacksize(memory_limit_attr, PTHREAD_STACK_MIN);
    pthread_attr_setguardsize(memory_limit_attr, 0);

    for (int i = 0; i < THREADS_NUM; ++i) {
        pthread_create(&threads[i], NULL, (void*)thread_func, NULL);
    }
    pthread_attr_destroy(memory_limit_attr);

    int global_sum = 0;
    for (int i = 0; i < THREADS_NUM; ++i) {
        int* sum;
        pthread_join(threads[i], (void**)&sum);
        // printf("%d\n", *sum);
        global_sum += *sum;
        free(sum);
    }
    printf("%d\n", global_sum);
    return 0;
}
