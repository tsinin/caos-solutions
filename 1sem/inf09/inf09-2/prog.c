#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    const int MAX_NUM_OF_WORDS = 255;
    const int MAX_WORD_LENGTH = 4096;
    int count = 0;
    pid_t process_id = 0;
    for (int i = 0; i < MAX_NUM_OF_WORDS; ++i) {
        process_id = fork();
        if (process_id != 0) {
            int status = 0;
            waitpid(process_id, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                break;
            } else if (WIFEXITED(status) && WEXITSTATUS(status) == 1) {
                ++count;
            }
        } else {
            char word[MAX_WORD_LENGTH];
            if (scanf("%s", word) != EOF) {
                exit(1);
            } else {
                exit(0);
            }
        }
    }
    printf("%d\n", count);
    return 0;
}
