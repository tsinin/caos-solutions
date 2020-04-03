#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        return 0;
    }
    int input = open(argv[1], O_RDONLY);
    if (input < 0) {
        return 1;
    }
    int value = 0;
    __uint32_t next_pointer = 0;
    off_t ptr = -1;
    while (ptr != 0) {
        if (read(input, &value, sizeof(value)) <= 0) {
            close(input);
            return 0;
        }
        if (read(input, &next_pointer, sizeof(next_pointer)) <= 0) {
            next_pointer = 0;
        }
        printf("%d ", value);
        ptr = lseek(input, next_pointer, SEEK_SET);
    }
    close(input);
    return 0;
}
