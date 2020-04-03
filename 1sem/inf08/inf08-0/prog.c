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
    if (argc <= 2) {
        return 0;
    }
    char* file_name = argv[1];
    char* str_to_find = argv[2];
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
    int str_len = strlen(str_to_find);
    char* tmp = memmem(mem_ptr, st.st_size, str_to_find, str_len);
    while (tmp != NULL) {
        printf("%ld ", tmp - mem_ptr);
        tmp++;
        if (st.st_size - (long)tmp + mem_ptr <= 0) {
            break;
        }
        tmp = memmem(
            tmp, (long)(st.st_size + mem_ptr - tmp), str_to_find, str_len);
    }
    printf("\n");
    munmap(mem_ptr, st.st_size);
    close(file_d);
    return 0;
}
