#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int N = atoi(argv[1]);
    pid_t process_id = 0;
    for (int i = 0; i < N; ++i) {
        process_id = fork();
        if (process_id != 0) {
            int tmp = 0;
            waitpid(process_id, &tmp, 0);
            if (i == 0) {
                printf("%d\n", N - i);
            } else {
                printf("%d ", N - i);
            }
            break;
        }
    }
    return 0;
}
