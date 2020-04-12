#include <stdio.h>
#include <fcntl.h>
#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
    char sem_name[1024];
    char shm_name[1024];
    int N;
    scanf("%s", sem_name);
    scanf("%s", shm_name);
    scanf("%d", &N);
    
    sem_t* semaphore = sem_open(sem_name, 0);
    sem_wait(semaphore);
    sem_close(semaphore);

    int shm_fd = shm_open(shm_name, O_RDONLY, 0);

    size_t map_size = N * sizeof(int);
    int* array = mmap(NULL, map_size,
                      PROT_READ,
                      MAP_SHARED,
                      shm_fd, 0);
    close(shm_fd);
    for (size_t i = 0; i < N; ++i) {
        printf("%d ", array[i]);
    }
    munmap(array, map_size);
    return 0;
}
