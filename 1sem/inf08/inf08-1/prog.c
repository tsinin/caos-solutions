#define _GNU_SOURCE
#include <fcntl.h>
#include <inttypes.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        return 0;
    }
    char* file_name = argv[1];
    int file_d = open(file_name, O_RDONLY);
    if (file_d == -1) {
        return 0;
    }
    struct stat st;
    int res = fstat(file_d, &st);
    if (res == -1) {
        close(file_d);
        return 0;
    }
    char* mem_ptr = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, file_d, 0);
    if (mem_ptr == MAP_FAILED) {
        return 0;
    }

    uint32_t ptr = -1;
    while (ptr != 0) {
        if (ptr == -1) {
            ptr = 0;
        }
        if (ptr >= st.st_size) {
            close(file_d);
            return 0;
        }
        // print value
        printf("%d ", *(int*)(mem_ptr + ptr));
        // read next_pointer
        ptr = *(uint32_t*)(mem_ptr + ptr + 4);
    }
    munmap(mem_ptr, st.st_size);
    close(file_d);
    printf("\n");
    return 0;
}
