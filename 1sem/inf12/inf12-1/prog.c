#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
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
    sprintf(
        prog,
        "#include <stdio.h>\n"
        "#include <math.h>\n\n"
        "int main() {\n"
        "\tint ans = (%s);\n"
        "\tprintf(\"%%d\\n\", ans);\n"
        "\treturn 0;\n"
        "}\n",
        c);
    // printf("%s", prog);
    int fd = open("programm.c", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    write(fd, prog, strlen(prog));
    close(fd);
    if (fork() == 0) {
        execlp("gcc", "gcc", "programm.c", NULL);
        perror("execute_gcc");
        return 1;
    }
    wait(NULL);
    if (fork() == 0) {
        execlp("./a.out", "./a.out", NULL);
        perror("execute_prog");
        return 2;
    }
    wait(NULL);
    if (fork() == 0) {
        execlp("rm", "rm", "programm.c", NULL);
        perror("execute_rm");
        return 3;
    }
    wait(NULL);
    return 0;
}                         // DONT NEED PROGRAM FOR REMOVING FILE
