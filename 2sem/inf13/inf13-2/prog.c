#define _GNU_SOURCE

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("There must be 1 argument.\n");
	exit(1);
    }
    int fds[2];
    pipe(fds);

    pid_t pid = fork();
    if (pid == 0) {
        dup2(fds[1], 2);
        close(fds[1]);
        execlp("gcc", "gcc", argv[1], NULL);
	perror("exec");
	exit(1);
    } else {
        close(fds[1]);
        ssize_t errors_num = 0;
        ssize_t warnings_num = 0;
        int last_str_error = -1;
        int last_str_warning = -1;
        ssize_t tmp_str;
        ssize_t tmp_symbol;

        char format[1000];
        format[0] = '\0';
        strcat(format, argv[1]);
        strcat(format, ":%u:%u:");

        char buff[1000];
        FILE* file = fdopen(fds[0], "r");

        while (fscanf(file, "%s", buff) > 0) {
            if (sscanf(buff, format, &tmp_str, &tmp_symbol) == 2) {
                fscanf(file, "%s", buff);
                if (strcmp(buff, "error:") == 0) {
                    if (tmp_str != last_str_error) {
                        errors_num++;
                        last_str_error = tmp_str;
                    }
                } else if (strcmp(buff, "warning:") == 0) {
                    if (tmp_str != last_str_warning) {
                        warnings_num++;
                        last_str_warning = tmp_str;
                    }
                }
            }
        }
        printf("%ld %ld\n", errors_num, warnings_num);
        close(fds[0]);
        fclose(file);
    }
}
