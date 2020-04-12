#include <pthread.h>
#include <stdio.h>


struct thread_arg {
    volatile double* arr;
    pthread_mutex_t* mutex;
    int N;
    int k;
    int num;
};

void* thread(void* arg_) {
    struct thread_arg* arg = (struct thread_arg*)arg_;
    int curr_num;
    for (size_t i = 0; i < arg->N; ++i) {
        pthread_mutex_lock(arg->mutex);
        curr_num = arg->num;
        arg->arr[curr_num] += 1;

        curr_num = (arg->num + 1) % arg->k;
        arg->arr[curr_num] += 1.01;

        curr_num = (arg->num - 1) % arg->k;
        if (curr_num < 0) {
            curr_num += arg->k;
        }
        arg->arr[curr_num] += 0.99;
        pthread_mutex_unlock(arg->mutex);
    }
    return NULL;
}


int main(int argc, char** argv) {
    if (argc != 3) {
        printf("The program takes 2 arguments.\n");
        return 1;
    }
    int N;
    int k;
    sscanf(argv[1], "%d", &N);
    sscanf(argv[2], "%d", &k);

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    volatile double arr[k];
    struct thread_arg args[k];
    for (int i = 0; i < k; ++i) {
        arr[i] = 0;
        args[i].arr = arr;
        args[i].mutex = &mutex;
        args[i].num = i;
        args[i].N = N;
        args[i].k = k;
    }

    pthread_t threads[k];
    for (size_t i = 0; i < k; ++i) {
        pthread_create(&threads[i], NULL, (void*)thread, &args[i]);
    }
    for (size_t i = 0; i < k; i++) {
        pthread_join(threads[i], NULL);
    }

    for (size_t i = 0; i < k; ++i) {
        printf("%.10g\n", arr[i]);
    }
    pthread_mutex_destroy(&mutex);
    return 0;
}
