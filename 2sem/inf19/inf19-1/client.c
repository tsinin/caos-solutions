#include <dlfcn.h>
#include <fcntl.h>
#include <inttypes.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef double (*libfunc)(double);

typedef struct {
    sem_t request_ready;  // начальное значение 0
    sem_t response_ready; // начальное значение 0
    char func_name[20];
    double value;
    double result;
} shared_data_t;

int main(int argc, char* argv[]) {
    char shm_name[250] = "/shm2_tsinin_diht82117";
    int shm_fd = shm_open(shm_name, O_RDWR, 0);
    size_t map_size = sizeof(shared_data_t);
    size_t page_size = sysconf(_SC_PAGESIZE);
    if (map_size % page_size > 0) {
        map_size = (map_size / page_size + 1) * page_size;
    }
    shared_data_t* data = mmap(NULL, map_size,
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED,
                      shm_fd, 0);
    int i = 0;
    while (i < 5) {
        data->value = i;
        strcpy(data->func_name, "sqr\0");
        sem_post(&data->request_ready);
        sem_wait(&data->response_ready);
        printf("%lf\n", data->result);
        i++;
    }

    i = 0;
    while (i < 5) {
        data->value = i;
        strcpy(data->func_name, "x2\0");
        sem_post(&data->request_ready);
        sem_wait(&data->response_ready);
        printf("%lf\n", data->result);
        i++;
    }
    strcpy(data->func_name, "\0");
    sem_post(&data->request_ready);
    close(shm_fd);
    munmap(data, map_size);
    return 0;
}
