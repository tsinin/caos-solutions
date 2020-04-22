#define _GNU_SOURCE
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <ifaddrs.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <netdb.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>


int main(int argc, char* argv[]) {
    if (argc != 4 || argv[2][0] != '/') {
        printf("Should be 2 arguments:\n"
               "1) domain name.\n"
               "2) script path with starting '/'.\n"
               "3) local file name.\n");
        return 1;
    }
    char* domain = argv[1];
    char* script_path = argv[2];
    char* file_path = argv[3];

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

    FILE* loc_file = fopen(file_path, "r");
    fseek(loc_file, 0L, SEEK_END);
    size_t file_size = ftell(loc_file);
    fclose(loc_file);

    char request[4096];
    size_t buf_size = 1024;
    char buf[buf_size];
    memset(request, '\0', 4096);
    memset(buf, '\0', buf_size);
    
    sprintf(request, 
            "POST %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %lu\r\n\r\n",
            script_path, domain, file_size);
    
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

    int file_fd = open(file_path, O_RDONLY);
    total_bytes = 0;
    while (total_bytes < file_size) {
        memset(buf, '\0', buf_size);
        tmp_bytes = read(file_fd, buf, buf_size);
        total_bytes += tmp_bytes;
        char* curr = buf;
        while (tmp_bytes > 0) {
            ssize_t tmp = send(sock_fd, curr, tmp_bytes, 0);
            tmp_bytes -= tmp;
            curr += tmp;
        }
    }

    total_bytes = 0;
    memset(buf, '\0', buf_size);
    if (read(sock_fd, buf, buf_size - 1) <= 0) {
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
        if ((tmp_bytes = read(sock_fd, buf, buf_size - 1)) < 0) {
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
