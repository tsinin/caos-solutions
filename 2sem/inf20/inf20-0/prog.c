#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <ifaddrs.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>

typedef struct ether_arp_packet {
    struct ether_header eth_hdr;
    struct ether_arp arp_hdr;
} PACKET;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        perror("There must be 1 argument.\n");
        return 1;
    }
    char* interface_name = argv[1];

    int sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if (sock_fd == -1) {
        perror("socket()");
        return 1;
    }

    struct sockaddr_ll sockaddr = {
        .sll_halen = ETH_ALEN
    };
    memset(sockaddr.sll_addr, 0xFF, 6);

    PACKET request, response;
    memset(&request, 0x00, sizeof(request));
    request.arp_hdr.ea_hdr.ar_hrd = htons(ARPHRD_ETHER);
    request.arp_hdr.ea_hdr.ar_pro = htons(ETH_P_IP);
    request.arp_hdr.ea_hdr.ar_hln = 0x06;
    request.arp_hdr.ea_hdr.ar_pln = 0x04;
    request.arp_hdr.ea_hdr.ar_op = htons(ARPOP_REQUEST);
    memset(request.arp_hdr.arp_tha, 0x00, 6);
    {
        struct ifreq ifr;

        strcpy((char*)&ifr.ifr_name, interface_name);
        ioctl(sock_fd, SIOCGIFHWADDR, &ifr);
        memcpy(request.arp_hdr.arp_sha, ifr.ifr_hwaddr.sa_data, 6);

        strcpy((char*)&ifr.ifr_name, interface_name);
        ioctl(sock_fd, SIOCGIFINDEX, &ifr);
        sockaddr.sll_ifindex = ifr.ifr_ifindex;
    }
    memset(request.eth_hdr.ether_dhost, 0xFF, 6);
    memcpy(request.eth_hdr.ether_shost, request.arp_hdr.arp_sha, 6);
    request.eth_hdr.ether_type = htons(ETH_P_ARP);

    char string_ip[20];
    while (scanf("%s", string_ip) != EOF) {
        if (inet_aton(string_ip, (struct in_addr*)request.arp_hdr.arp_tpa) == 0) {
            perror("inet_aton()");
            continue;
        }
        if (sendto(sock_fd, &request, sizeof(request), 0, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
            perror("sendto()");
            return 1;
        }
        if (recv(sock_fd, &response, sizeof(response), 0) < 0) {
            perror("recv()");
            return 1;
        }
        printf("%02x:%02x:%02x:%02x:%02x:%02x\n", response.arp_hdr.arp_sha[0], response.arp_hdr.arp_sha[1],
                                                  response.arp_hdr.arp_sha[2], response.arp_hdr.arp_sha[3],
                                                  response.arp_hdr.arp_sha[4], response.arp_hdr.arp_sha[5]);
    }
    close(sock_fd);
    return 0;
}
