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
 
char* query;
 
union integer{
  int uint;
  char bytes[4];
};
 
int parse(char* to_parse){
  char* prev_ptr = to_parse;
  char* new_ptr;
  int pos = 12;
  while ((new_ptr = strchr(prev_ptr, '.'))) {
    *new_ptr = '\0';
    union integer len;
    len.uint = strlen(prev_ptr);
    memcpy(query + pos, len.bytes, 1);
    ++pos;
    memcpy(query + pos, prev_ptr, strlen(prev_ptr));
    pos += strlen(prev_ptr);
    prev_ptr = new_ptr + 1;
  }
  new_ptr = prev_ptr + strlen(prev_ptr);
  *new_ptr = '\0';
  union integer len;
  len.uint = strlen(prev_ptr);
  memcpy(query + pos, len.bytes, 1);
  ++pos;
  memcpy(query + pos, prev_ptr, strlen(prev_ptr));
  pos += strlen(prev_ptr);
  prev_ptr = new_ptr + 1;
  *(query + pos + 2) = '\x01';
  *(query + pos + 4) = '\x01';
  return pos + 5;
}
 
int main(int argc, char *argv[])
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    char* to_resolve = (char*) malloc(1024);
    struct in_addr server_in_addr = {
        .s_addr = 0,
    };
    query = (char*) malloc(2048);
    inet_aton("8.8.8.8", &server_in_addr);
    while (scanf("%s", to_resolve) != EOF) {
      memset(query, '\x00', 2048);
      memset(query, '\x01', 3);
      *(query + 5) = '\x01';
      int len = parse(to_resolve);
      struct sockaddr_in server_sockaddr = {
        .sin_family = AF_INET,
        .sin_addr = server_in_addr,
        .sin_port = htons(53),
      };
      unsigned char* response = (unsigned char*) malloc(1024);
      sendto(sock, query, len, 0, (struct sockaddr*)&server_sockaddr, sizeof(struct sockaddr_in));
      int recv_size = recvfrom(sock, response, 1024, 0, NULL, NULL);
      response = response + (recv_size - 4);
      for (int i = 0; i < 3; ++i) {
        printf("%d.", response[i]);
      }
      printf("%d\n", response[3]);
      *(query + 22) = '\0';
    }
    close(sock);
    return 0;
}
