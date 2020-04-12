#include <stdio.h>
#include <pthread.h>
#include <malloc.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

void* minus_three(int* sock_pair) {
    while (1) {
        int num;
        int ret = read(sock_pair[0], &num, 4);
        if (ret <= 0) {
            return NULL;
        }
        num -= 3;
        if (num == 0 || num > 100) {
            printf("%d\n", num);
            close(sock_pair[0]);
            close(sock_pair[1]);
            return NULL;
        }
        printf("%d\n", num);
        write(sock_pair[0], &num, 4);
    }
}

void* plus_five(int* sock_pair) {
    while (1) {
        int num;
        int ret = read(sock_pair[1], &num, 4);
        if (ret <= 0) {
            return NULL;
        }
        num += 5;
        if (num == 0 || num > 100) {
            printf("%d\n", num);
            close(sock_pair[0]);
            close(sock_pair[1]);
            return NULL;
        }
        printf("%d\n", num);
        write(sock_pair[1], &num, 4);
    }
}

int main(int argc, char** argv) {
    int start_number;
    sscanf(argv[1], "%d", &start_number);
    pthread_t threads[2];

    int sock_pair[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sock_pair);
    write(sock_pair[1], &start_number, 4);

    pthread_create(&threads[0], NULL, (void*)minus_three, sock_pair);
    pthread_create(&threads[1], NULL, (void*)plus_five, sock_pair);
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    return 0;
} 
