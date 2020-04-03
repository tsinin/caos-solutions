#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

sig_atomic_t counter = 0;
sig_atomic_t end = 0;

void signal_handler(int sig_num)
{
    if (sig_num == SIGINT) {
        ++counter;
    } else if (sig_num == SIGTERM) {
        end = 1;
    }
};

int main()
{
    struct sigaction sig_int_struct;
    struct sigaction sig_term_struct;

    sig_int_struct.sa_handler = signal_handler;
    sigemptyset(&sig_int_struct.sa_mask);
    sig_int_struct.sa_flags = 0;
    sig_term_struct.sa_handler = signal_handler;
    sigemptyset(&sig_term_struct.sa_mask);
    sig_term_struct.sa_flags = 0;

    sigaction(SIGINT, &sig_int_struct, NULL);
    sigaction(SIGTERM, &sig_term_struct, NULL);

    pid_t process_id = getpid();
    printf("%d\n", process_id);
    fflush(stdout);

    counter = 0;
    while (!end) {
        sched_yield();
    }
    printf("%d\n", counter);
    return 0;
}
