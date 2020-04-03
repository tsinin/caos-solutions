#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    char file_name[PATH_MAX];
    struct stat st;
    char* end_ptr;
    char real_name[PATH_MAX];
    char link_name[PATH_MAX];
    while (fgets(file_name, sizeof(file_name), stdin)) {
        end_ptr = memchr(file_name, '\n', sizeof(file_name));
        if (end_ptr != NULL) {
            *end_ptr = '\0';
        }
        if (lstat(file_name, &st) == -1) {
            continue;
        }
        if (S_ISLNK(st.st_mode)) {
            // print real ABSOLUTE name of the file
            char* real_name_ptr = realpath(file_name, real_name);
            if (real_name_ptr != NULL) {
                puts(real_name);
            }
        } else if (S_ISREG(st.st_mode)) {
            // create a link with "link_to_" prefix
            char* base = basename(file_name);
            strcpy(link_name, "link_to_");
            strcat(link_name, base);
            symlink(file_name, link_name);
        }
    }
    return 0;
}
