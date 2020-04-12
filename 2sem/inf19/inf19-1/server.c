#define _GNU_SOURCE
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
    char* libfile_name = argv[1];
    void* dyn_lib = dlopen(libfile_name, RTLD_LAZY);
    if (dyn_lib == NULL) {
        fprintf(stderr, "dlopen error: %s\n", dlerror());
        return 1;
    }
    char shm_name[250] = "/shm2_tsinin_diht82117-212205042020";
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0777);
    ftruncate(shm_fd, sizeof(shared_data_t));
    shared_data_t* data = mmap(NULL, 
			sizeof(shared_data_t),
                      	PROT_READ | PROT_WRITE,
                      	MAP_SHARED,
                      	shm_fd, 0);
    close(shm_fd);

    shared_data_t new_data;
    *data = new_data;
    sem_init(&data->request_ready, 1, 0);
    sem_init(&data->response_ready, 1, 0);
    printf("%s\n", shm_name);
    fflush(stdout);

    while (1) {
        sem_wait(&data->request_ready);
        if (strlen(data->func_name) == 0) {
            break;
        }
        libfunc func = dlsym(dyn_lib, data->func_name);
        if (func == NULL) {
            fprintf(stderr, "dlsym error: %s\n", dlerror());
            return 1;
        }
        data->result = func(data->value);
        sem_post(&data->response_ready);
    }

    sem_destroy(&data->request_ready);
    sem_destroy(&data->response_ready);
    munmap(data, sizeof(shared_data_t));
    shm_unlink(shm_name);
    dlclose(dyn_lib);
    return 0;
}
