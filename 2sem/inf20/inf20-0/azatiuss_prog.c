#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
 
typedef struct {
  struct ether_header ethernet_header;
  struct ether_arp arp_packet;
}ArpRequest;
 
int main(int argc, char *argv[]) {
  int sock = socket(
        AF_PACKET,
        SOCK_RAW,
        htons(ETH_P_ARP)
    );
    struct ifreq ifr;
    strcpy((char*)&ifr.ifr_name, argv[1]);
    ioctl(sock, SIOCGIFINDEX, &ifr);
    struct sockaddr_ll device = {
        .sll_ifindex = ifr.ifr_ifindex,
        .sll_halen = ETH_ALEN,
    };
    ioctl(sock, SIOCGIFHWADDR, &ifr);
    ArpRequest request;
    memset(&request, '\x00', sizeof(request));
    memcpy(request.arp_packet.arp_sha, ifr.ifr_hwaddr.sa_data, 6);
    memcpy(request.ethernet_header.ether_shost, ifr.ifr_hwaddr.sa_data, 6);
    request.ethernet_header.ether_type = 0x0608;
    request.arp_packet.ea_hdr.ar_hrd = 0x0100;
    request.arp_packet.ea_hdr.ar_pro = 0x0008;
    request.arp_packet.ea_hdr.ar_hln = 0x0006;
    request.arp_packet.ea_hdr.ar_pln = 0x0004;
    request.arp_packet.ea_hdr.ar_op = 0x0100;
    memset(device.sll_addr, '\xff', 6);
    memset(request.ethernet_header.ether_dhost, '\xff', 6);
    char ip[20];
    while (scanf("%s", ip) != EOF) {
      inet_aton(ip, (struct in_addr *)request.arp_packet.arp_tpa);
      ArpRequest response;
      sendto(sock, &request, sizeof(request), 0, (struct sockaddr*)&device, sizeof(device));
      recv(sock, &response, sizeof(response), 0);
      printf("%02x:%02x:%02x:%02x:%02x:%02x\n", response.arp_packet.arp_sha[0], response.arp_packet.arp_sha[1],
                                                response.arp_packet.arp_sha[2], response.arp_packet.arp_sha[3],
                                                response.arp_packet.arp_sha[4], response.arp_packet.arp_sha[5]);
    }
}
