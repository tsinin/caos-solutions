#include <fcntl.h>
#include <inttypes.h>
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
    struct stat st, interpr;
    char* end_ptr;
    while (fgets(file_name, sizeof(file_name), stdin)) {
        end_ptr = memchr(file_name, '\n', sizeof(file_name));
        if (end_ptr != NULL) {
            *end_ptr = '\0';
        }

        int file_d = open(file_name, O_RDONLY);
        if ((file_d > 0) && (fstat(file_d, &st) != -1) &&
            ((st.st_mode & S_IXUSR) != 0)) {
            if (lseek(file_d, 0, SEEK_END) < 2) {
                puts(file_name);
                continue;
            }
            lseek(file_d, 0, SEEK_SET);
            char check_begin[2];
            if (read(file_d, check_begin, sizeof(char) * 2) == -1) {
                puts(file_name);
                continue;
            }
            if (check_begin[0] == '#' && check_begin[1] == '!') {
                char interpr_name[PATH_MAX];
                if (read(file_d, interpr_name, sizeof(interpr_name)) == -1) {
                    puts(file_name);
                    continue;
                }
                end_ptr = memchr(interpr_name, '\n', sizeof(interpr_name));
                if (end_ptr != NULL) {
                    *end_ptr = '\0';
                }
                if ((stat(interpr_name, &interpr) == -1) ||
                    ((interpr.st_mode & S_IXUSR) == 0)) {
                    puts(file_name);
                }
            } else {
                lseek(file_d, 0, SEEK_SET);
                char check_elf[4];
                if (read(file_d, check_elf, sizeof(char) * 4) == -1) {
                    puts(file_name);
                    continue;
                }
                if (check_elf[0] != 0x7f || check_elf[1] != 'E' ||
                    check_elf[2] != 'L' || check_elf[3] != 'F') {
                    puts(file_name);
                }
            }
        }
    }
    return 0;
}
