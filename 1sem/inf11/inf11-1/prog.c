#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

sig_atomic_t pid = 0;
sig_atomic_t value = 0;
sig_atomic_t end = 0;

void signal_handler(int sig_num, siginfo_t* sig_info, void* context)
{
    if (sig_info->si_value.sival_int == 0) {
        end = 1;
        return;
    }
    pid = sig_info->si_pid;
    value = sig_info->si_value.sival_int - 1;
};

int main(int argc, char* argv[])
{
    sigset_t ignored;
    sigfillset(&ignored);
    sigdelset(&ignored, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &ignored, NULL);

    struct sigaction sig_rtmin_struct;

    sig_rtmin_struct.sa_sigaction = signal_handler;
    sig_rtmin_struct.sa_flags = SA_SIGINFO;
    sigemptyset(&sig_rtmin_struct.sa_mask);

    sigaction(SIGRTMIN, &sig_rtmin_struct, NULL);

    while (!end) {
        while (pid == 0 && !end) {
            sched_yield();
        }
        if (!end) {
            union sigval to_send;
            to_send.sival_int = value;
            sigqueue(pid, SIGRTMIN, to_send);
            pid = 0;
        }
    }
    return 0;
}
