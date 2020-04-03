#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    int limit = 1;
    pid_t process_id = fork();
    while (process_id == 0) {
        if (process_id != -1) {
            ++limit;
        }
        process_id = fork();
    }
    if (process_id > 0) {
        int tmp = 0;
        waitpid(process_id, &tmp, 0);
    }
    if (process_id == -1) {
        printf("%d\n", limit);
    }
    return 0;
}
