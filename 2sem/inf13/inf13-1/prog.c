#define _GNU_SOURCE

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int N = argc - 1;

    int fds_in[2];
    int fds_out[2];
    pipe2(fds_in, O_NONBLOCK);
    pipe2(fds_out, O_NONBLOCK);

    for (int i = 0; i < N; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            if (i > 0) {
                // read from pipe_out
                dup2(fds_out[0], 0);
                close(fds_out[0]);
            }
            if (i < N - 1) {
                // write in pipe_in 
                dup2(fds_in[1], 1);
                close(fds_in[1]);
            }
            execlp(argv[i + 1], argv[i + 1], NULL);
        } else {
            wait(0);
            if (i < N - 1) {
                char buff[1000];
                int tmp_size;
                while((tmp_size = read(fds_in[0], buff, sizeof(buff))) > 0) {
                    write(fds_out[1], buff, tmp_size);
                }
            }
        }
    }
    close(fds_in[0]);
    close(fds_in[1]);
    close(fds_out[0]);
    close(fds_out[1]);
}
