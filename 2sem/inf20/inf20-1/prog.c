#define _GNU_SOURCE
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <ifaddrs.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>

size_t pack_domain_to_query(char* query, char* domain, int dom_len) {
    char* start = query + 12;
    char* curr;
    char* prev = domain - 1;
    uint8_t len;
    while (curr = strchrnul(prev + 1, '.')) {
        len = (uint8_t)(curr - prev - 1);
        memcpy(start, &len, 1);
        start++;
        memcpy(start, prev + 1, len);
        start += len;
        prev = curr;
        if (*curr == '\0') {
            len = 0;
            memcpy(start, &len, 1);
            start++;
            break;
        }
    }
    char tocpy[4] = "\x00\x01\x00\x01";
    memcpy(start, tocpy, 4);
    start += 4;
    return start - query;
}

int main(int argc, char* argv[]) {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("socket()");
        return 1;
    }

    struct sockaddr_in sockaddr = {
        .sin_family = AF_INET,
        .sin_port = htons(53)
    };
    inet_aton("8.8.8.8", &sockaddr.sin_addr);

    uint8_t request[4096];
    uint8_t response[4096];
    

    char domain[200];
    while (scanf("%s", domain) != EOF) {
        memset(request, 0x00, 4096);
        char tocpy[7] = "\xAB\xCD\x01\x00\x00\x01";
        memcpy(request, tocpy,6);
        // copy domain to request and pack the request
        int len = pack_domain_to_query(request, domain, strlen(domain));
        if (sendto(sock_fd, request, len, 0, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
            perror("sendto()");
            return 1;
        }
        int response_size;
        if ((response_size = recv(sock_fd, response, 4096, 0)) < 0) {
            perror("recv()");
            return 1;
        }
        uint8_t* ans = response + (response_size - 4);
        for (int i = 0; i < 3; ++i) {
            printf("%d.", ans[i]);
        }
        printf("%d\n", ans[3]);
    }
    close(sock_fd);
    return 0;
}
