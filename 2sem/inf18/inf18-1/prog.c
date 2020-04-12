#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

typedef double (*function_t)(double);

double* pmap_process(function_t func, const double *in, size_t count) {
    double* out = mmap(NULL, 
                        count * sizeof(double),
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS,
                        -1, 
                        0);
    // shared memory mapping for shared unnamed semaphore
    sem_t* proc_ready = mmap(NULL,
                            sizeof(sem_t),
                            PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS,
                            -1,
                            0);
    sem_init(proc_ready, 1, 0);
    size_t procnum = get_nprocs();
    pid_t workers[procnum];
    for (size_t i = 0; i < procnum; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            for (size_t ind = i; ind < count; ind += procnum) {
                out[ind] = func(in[ind]);
            }
            sem_post(proc_ready);
            sem_close(proc_ready);
            exit(0);
        }
        workers[i] = pid;
    }
    for (size_t i = 0; i < procnum; ++i) {
        sem_wait(proc_ready);
    }
    sem_close(proc_ready);
    for (size_t i = 0; i < procnum; ++i) {
        waitpid(workers[i], NULL, 0);
    }
    munmap(proc_ready, sizeof(proc_ready));
    return out;
}

void pmap_free(double *ptr, size_t count) {
    munmap(ptr, count * sizeof(double));
}
