#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int input = open(argv[1], O_RDONLY);
    if (input < 0) {
        close(input);
        return 1;
    }
    int output1 =
        open(argv[2], O_WRONLY | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
    int output2 =
        open(argv[3], O_WRONLY | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
    if (output1 < 0 || output2 < 0) {
        close(input);
        close(output1);
        close(output2);
        return 2;
    }
    char buf[1000];
    char output_buf1[1000];
    char output_buf2[1000];
    memset(output_buf1, '\0', 1000);
    memset(output_buf2, '\0', 1000);
    ssize_t readed;
    int written = 0;
    while ((readed = read(input, buf, 1000)) > 0) {
        for (int i = 0; i < readed; ++i) {
            if (buf[i] >= '0' && buf[i] <= '9') {
                // write in correct buffer
                output_buf1[strlen(output_buf1)] = buf[i];
            } else {
                // write in correct buffer
                output_buf2[strlen(output_buf2)] = buf[i];
            }
            written = 0;
            if (strlen(output_buf1) == 999) {
                written = write(output1, output_buf1, 999);
                memset(output_buf1, '\0', 1000);
            }
            if (strlen(output_buf2) == 999) {
                written = write(output2, output_buf2, 999);
                memset(output_buf2, '\0', 1000);
            }
            if (written < 0) {
                close(input);
                close(output1);
                close(output2);
                return 3;
            }
        }
    }
    written = 0;
    if (strlen(output_buf1) > 0) {
        written = write(output1, output_buf1, strlen(output_buf1));
    }
    if (strlen(output_buf2) > 0) {
        written = write(output2, output_buf2, strlen(output_buf2));
    }
    if (written < 0) {
        close(input);
        close(output1);
        close(output2);
        return 3;
    }
    close(input);
    close(output1);
    close(output2);

    if (readed < 0) {
        return 3;
    } else {
        return 0;
    }
}
