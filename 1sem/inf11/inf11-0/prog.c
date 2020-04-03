#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    FILE** files = NULL;
    int N = argc - 1;

    files = malloc(sizeof(FILE*) * N);
    for (int i = 0; i < N; ++i) {
        if ((files[i] = fopen(argv[i + 1], "r")) == NULL) {
            for (int j = 0; j < i; ++j) {
                fclose(files[j]);
            }
            free(files);
            return 1;
        }
    }

    sigset_t handler;
    sigset_t ignored;
    sigemptyset(&handler);
    for (int i = 0; i < N + 1; ++i) {
        sigaddset(&handler, SIGRTMIN + i);
    }
    sigfillset(&ignored);
    int signals_fd = signalfd(-1, &handler, 0);
    sigprocmask(SIG_BLOCK, &ignored, NULL);

    struct signalfd_siginfo signal_info;

    char* line;
    while (1) {
        read(signals_fd, &signal_info, sizeof(signal_info));
        int sig_num = signal_info.ssi_signo;
        if (sig_num == SIGRTMIN) {
            break;
        }
        line = NULL;
        ssize_t size = 0;
        getline(&line, &size, files[sig_num - SIGRTMIN - 1]);
        printf("%s", line);
        fflush(stdout);
        free(line);
    }
    for (int i = 0; i < N; ++i) {
        fclose(files[i]);
    }
    free(files);
    return 0;
}
