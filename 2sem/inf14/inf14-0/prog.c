#include <arpa/inet.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("There must be 2 arguments only\n");
        return 1;
    }
    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    uint16_t port;
    sscanf(argv[2], "%" SCNd16 "", &port);
    sockaddr.sin_port = htons(port);
    sockaddr.sin_addr.s_addr = inet_addr(argv[1]);

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        printf("Error caught while opening socket.\n");
        return 1;
    }
    if (connect(sock_fd, (const struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
        printf("Connection error.\n");
        return 1;
    }

    int tmp;
    while (1) {
        if (scanf("%d", &tmp) <= 0) {
            break;
        }
        if (write(sock_fd, &tmp, sizeof(int)) <= 0) {
            break;
        }
        if (read(sock_fd, &tmp, sizeof(int)) <= 0) {
            break;
        }
        printf("%d\n", tmp);
    }
    shutdown(sock_fd, SHUT_RDWR);
    close(sock_fd);
    return 0;
}
