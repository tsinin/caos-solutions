#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

sig_atomic_t value = 0;
sig_atomic_t end = 0;

void signal_handler(int sig_num)
{
    if (sig_num == SIGUSR1) {
        ++value;
    } else if (sig_num == SIGUSR2) {
        value *= -1;
    } else if (sig_num == SIGTERM) {
        end = 1;
        return;
    }
    printf("%d\n", value);
    fflush(stdout);
};

int main()
{
    struct sigaction sig_usr1_struct;
    struct sigaction sig_usr2_struct;
    struct sigaction sig_term_struct;

    sig_usr1_struct.sa_handler = signal_handler;
    sigemptyset(&sig_usr1_struct.sa_mask);
    sig_usr1_struct.sa_flags = 0;
    sig_usr2_struct.sa_handler = signal_handler;
    sigemptyset(&sig_usr2_struct.sa_mask);
    sig_usr2_struct.sa_flags = 0;
    sig_term_struct.sa_handler = signal_handler;
    sigemptyset(&sig_term_struct.sa_mask);
    sig_term_struct.sa_flags = 0;

    sigaction(SIGUSR1, &sig_usr1_struct, NULL);
    sigaction(SIGUSR2, &sig_usr2_struct, NULL);
    sigaction(SIGTERM, &sig_term_struct, NULL);

    pid_t process_id = getpid();
    printf("%d\n", process_id);
    fflush(stdout);
    scanf("%d", &value);

    end = 0;
    while (!end) {
        sched_yield();
    }
    return 0;
}
