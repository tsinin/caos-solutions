#include <stdio.h>
#include <unistd.h>       
#include <sys/types.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
 
int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("There must be 2 arguments.\n");
        return 1;
    }
    char* command = argv[1];
    char* in_file_name = argv[2];
 
    int fds[2];
    pipe(fds);

    pid_t pid = fork();
    if (pid == 0) {
        int in_file = open(in_file_name, O_RDONLY);
        dup2(in_file, 0);
        close(in_file);

        dup2(fds[1], 1);
        close(fds[1]);
        execlp(command, command, NULL);
    } else {
        close(fds[1]);
        uint64_t total_size = 0;
        uint64_t tmp_size;
        char buff[1000];
        while((tmp_size = read(fds[0], buff, sizeof(buff))) > 0) {
            total_size += tmp_size;
        }
        printf("%ld\n", total_size);
    }
}
