#include <stdio.h>
#include <pthread.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdatomic.h>

typedef struct Item {
    struct Item* next;
    int64_t      value;
} item_t;

struct thread_arg {
    struct Item* head;
    int64_t k;
    int64_t thread_num;
};

void* thread_func(void* arg_) {
    struct thread_arg* arg = (struct thread_arg*)arg_;
    for (int i = 0; i < arg->k; ++i) {
        struct Item* new_item = malloc(sizeof(struct Item));
        struct Item* null_item = NULL;
        new_item->next = NULL;
        new_item->value = arg->k * arg->thread_num + i;
        while (!atomic_compare_exchange_strong(&arg->head->next, 
                                               &null_item, 
                                               new_item)) {
            arg->head = arg->head->next;
            null_item = NULL;
        }
        sched_yield();
    }
    return NULL;
}

int main(int argc, char** argv) {
    int N, k;
    sscanf(argv[1], "%d", &N);
    sscanf(argv[2], "%d", &k);

    struct Item* head = malloc(sizeof(struct Item));
    head->next = NULL;
    head->value = 0;

    struct thread_arg args[N];
    for (int i = 0; i < N; ++i) {
        args[i].head = head;
        args[i].k = k;
        args[i].thread_num = i;
    }

    pthread_t thread[N];
    for (int i = 0; i < N; ++i) {
        pthread_create(&thread[i], NULL, thread_func, &args[i]);
    }
    for (int i = 0; i < N; ++i) {
        pthread_join(thread[i], NULL);
    }

    head = head->next;
    while (head != NULL) {
        struct Item* prev = head;
        printf("%ld ", head->value);
        head = head->next;
        free(prev);
    }
    printf("\n");
    return 0;
}
