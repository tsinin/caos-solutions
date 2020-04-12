#include <stdio.h>
#include <inttypes.h>
#include <pthread.h>

struct thread_arg {
    pthread_mutex_t* mutex;
    pthread_cond_t* condvar;
    uint64_t A;
    uint64_t B;
    uint64_t* num;
};

int is_prime(int num) {
    if (num <= 1) {
        return 0;
    }
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) {
            return 0;
        }
    }
    return 1;
}

void* thread_func(void* arg_) {
    struct thread_arg* arg = (struct thread_arg*)arg_;
    for (uint64_t i = arg->A; i <= arg->B; ++i) {
        if (is_prime(i)) {
            *arg->num = i;
            pthread_cond_signal(arg->condvar);
            pthread_cond_wait(arg->condvar, arg->mutex);
            if (*arg->num == 0) {
                break;
            }
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    uint64_t A, B, N;
    sscanf(argv[1], "%ld", &A);
    sscanf(argv[2], "%ld", &B);
    sscanf(argv[3], "%ld", &N);

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_t condvar;
    pthread_cond_init(&condvar, NULL);

    uint64_t prime_num = 0;
    struct thread_arg arg;
    arg.mutex = &mutex;
    arg.condvar = &condvar;
    arg.A = A;
    arg.B = B;
    arg.num = &prime_num;

    pthread_mutex_lock(&mutex);

    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, &arg);

    int prime_numbers = 0;
    while (prime_numbers != N) {
        pthread_cond_wait(&condvar, &mutex);
        printf("%ld\n", prime_num);
        prime_numbers++;
        if (prime_numbers == N) {
            prime_num = 0;
        } else {
            prime_num = 1;
        }
        pthread_cond_signal(&condvar);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condvar);
}
