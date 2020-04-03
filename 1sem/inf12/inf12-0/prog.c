#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    char c[5000];
    fgets(c, 5000, stdin);
    if (c[strlen(c) - 1] == '\n') {
        c[strlen(c) - 1] = '\0';
    }
    if (strlen(c) == 0) {
        return 0;
    }
    char prog[5000];
    sprintf(prog, "str = %s\nprint(str)\n", c);
    // printf("%s", prog);
    execlp("python3", "python3", "-c", prog, NULL);
    perror("execute");
    return 1;
}
