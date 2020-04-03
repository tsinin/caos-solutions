#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sched.h>
#include <signal.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <ctype.h>
 
volatile int sig_sent = 0;
volatile int now_processed = 0;
 
void handle_sig(int sigusr_id)
{
    sig_sent = 1;
    if (!now_processed) {
      exit(0);
    }
}
 
int main(int argc, char** argv)
{
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  int flags = fcntl(sock_fd, F_GETFL);
  flags |= O_NONBLOCK;
  fcntl(sock_fd, F_SETFL, flags);
  struct in_addr server_in_addr = {
        .s_addr = 0,
    };
  inet_aton("127.0.0.1", &server_in_addr);
  struct sockaddr_in server_sockaddr = {
        .sin_family = AF_INET,
        .sin_addr = server_in_addr,
        .sin_port = htons(strtoll(argv[1], NULL,10)),
    };
  bind(sock_fd, (const struct sockaddr*)&server_sockaddr, sizeof(struct sockaddr_in));
  listen(sock_fd, 1);
  struct sigaction sig_handler;
  sigemptyset(&sig_handler.sa_mask);
  sig_handler.sa_handler = handle_sig;
  sig_handler.sa_flags = SA_RESTART;
  sigaction(SIGTERM, &sig_handler, NULL);
  int epoll_fd = epoll_create1(0);
  struct epoll_event events[4096];
  struct epoll_event event;
  event.events = EPOLLIN;
  event.data.fd = sock_fd;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &event);
  int to_process;
  char buffer[4096];
  while (!sig_sent) {
    to_process = epoll_wait(epoll_fd, events, 4096, -1);
    now_processed = 1;
    for (int i = 0; i < to_process; ++i) {
      if (events[i].data.fd != sock_fd) {
          int ct = read(events[i].data.fd, buffer, sizeof(buffer));
          if (!ct) {
            shutdown(events[i].data.fd, 2);
            close(events[i].data.fd);
            continue;
          }
          for (int i = 0; i < ct; ++i) {
            buffer[i] = toupper(buffer[i]);
          }
          write(events[i].data.fd, buffer, ct);
      } else {
          int client_fd = accept(sock_fd, NULL, NULL);
          flags = fcntl(client_fd, F_GETFL);
          flags |= O_NONBLOCK;
          fcntl(client_fd, F_SETFL, flags);
          struct epoll_event tmp_ev;
          tmp_ev.events = EPOLLIN;
          tmp_ev.data.fd = client_fd;
          epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &tmp_ev);
      }
    }
    now_processed = 0;
  }
  shutdown(sock_fd, 2);
  close(sock_fd);
  return 0;
}
