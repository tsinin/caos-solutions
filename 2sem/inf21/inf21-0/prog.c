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

#include <netdb.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>


int main(int argc, char* argv[]) {
    if (argc != 3 || argv[2][0] != '/') {
        printf("Should be 2 arguments:\n1) domain name\n2) file path with starting '/'.\n");
        return 1;
    }
    char* domain = argv[1];
    char* path = argv[2];

    struct protoent* tcp_entry = getprotobyname("tcp");
    if (tcp_entry == NULL) {
        perror("getprotoent()");
        return 1;
    }
    endprotoent();

    int sock_fd = socket(AF_INET, SOCK_STREAM, tcp_entry->p_proto);
    if (sock_fd == -1) {
        perror("socket()");
        return 1;
    }

    struct hostent* host_entry = gethostbyname(domain);
    if (host_entry == NULL) {
        perror("gethostbyname()");
        return 1;
    }

    struct sockaddr_in servaddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*(host_entry->h_addr_list))),
        .sin_port = htons(80),
    };

    if (connect(sock_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) { 
        perror("connect()"); 
        return 1; 
    } 

    char request[4096];
    char buf[1024];
    size_t buf_size = 1023;
    
    sprintf(request, 
            "GET %s HTTP/1.1\r\n"
            "Host: %s\r\n\r\n",
            path, domain);
    size_t request_len = strlen(request);
    size_t total_bytes = 0;
    ssize_t tmp_bytes;

    while (total_bytes < request_len) {
        tmp_bytes = send(sock_fd, 
                         request + total_bytes, 
                         strlen(request) - total_bytes, 
                         0);
        if (tmp_bytes < 0) {
            perror("send()");
            return 1;
        }
        total_bytes += tmp_bytes;
    }
    total_bytes = 0;
    /* while ((tmp_bytes = read(
                             sock_fd, 
                             response + total_bytes, 
                             sizeof(response) - total_bytes)
                            ) > 0) {
        total_bytes += tmp_bytes;
    } */
    if (read(sock_fd, buf, buf_size) <= 0) {
        perror("read()");
        return 1;
    }
    char* len_ptr = strstr(buf, "Content-Length: ") + 16;
    uint64_t content_length;
    sscanf(len_ptr, "%lu", &content_length);

    char* ans = strstr(buf, "\r\n\r\n") + 4;
    printf("%s", ans);
    content_length -= strlen(ans);
    while (content_length > 0) {
        if ((tmp_bytes = read(sock_fd, buf, buf_size)) < 0) {
            perror("read()");
            return 1;
        }
        buf[tmp_bytes] = '\0';
        printf("%s", buf);
        content_length -= tmp_bytes;
    }
    close(sock_fd);
    return 0;
}
