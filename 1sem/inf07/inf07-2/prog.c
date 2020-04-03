#include <inttypes.h>
#include <limits.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    uint64_t summary_size = 0;
    char file_name[PATH_MAX];
    struct stat status;
    char* stop_ptr;
    while (fgets(file_name, sizeof(file_name), stdin)) {
        stop_ptr = memchr(file_name, '\n', sizeof(file_name));
        if (stop_ptr != NULL) {
            *stop_ptr = '\0';
        }
        int res = lstat(file_name, &status);
        if (res != -1 && S_ISREG(status.st_mode)) {
            summary_size += status.st_size;
        }
    }
    printf("%" PRIu64 "\n", summary_size);
    return 0;
}
