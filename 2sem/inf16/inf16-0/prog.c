#include <stdio.h>
#include <pthread.h>
#include <malloc.h>

void* thread_func(void* v) {
    int num;
    int ret = scanf("%d", &num);
    if (ret < 1) {
        return NULL;
    }
    pthread_t thread;
    pthread_create(&thread, NULL, (void*)thread_func, NULL);
    pthread_join(thread, NULL);
    printf("%d\n", num);
}

int main() {
    pthread_t thread;
    if (pthread_create(&thread, NULL, (void*)thread_func, NULL) < 0) {
        printf("Error.\n");
        return 1;
    }
    pthread_join(thread, NULL);
    return 0;
}
