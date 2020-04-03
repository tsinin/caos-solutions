#include <arpa/inet.h>
#include <ctype.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

enum {
    MAX_CONNECTIONS = 65536,
    MAX_EVENTS = 65536
};

volatile sig_atomic_t end = 0;

void signal_handler(int sig_num)
{
    if (sig_num == SIGTERM) {
        end = 1;
    }
}

typedef struct {
    int fd;
    int closed;
} data_t;

ssize_t write_all(int fd, void* buf, size_t sz) {
    while (sz > 0) {
        int ret = write(fd, buf, sz);
        if (ret < 0) {
            return ret;
        }    
        buf += ret;
        sz -= ret;
    }
    return sz;
}

void
shutdown_clnt(int fd) {
    shutdown(fd, SHUT_RDWR);
    close(fd);
}

void
process_clnt(struct epoll_event *event)
{
    data_t* data_ptr = (data_t*)(event->data.ptr);
    if (event->events & EPOLLIN) {
        char buff[4096];
        ssize_t readed = read(data_ptr->fd, buff, 4096);
        if (readed == 0) {
            shutdown_clnt(data_ptr->fd);
            data_ptr->closed = 1;
        }
        for (int i = 0; i < readed; ++i) {
            buff[i] = toupper(buff[i]);
        }
        write_all(data_ptr->fd, buff, readed);
    }
}

data_t*
create_event_data(int epoll_fd, int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
    data_t* data = malloc(sizeof(data_t));
    data->fd = fd;
    data->closed = 0;

    struct epoll_event event_ready_read;
    event_ready_read.events = EPOLLIN;
    event_ready_read.data.ptr = data;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event_ready_read) == -1) {
        printf("Error occured while creating an event.\n");
        exit(1);
    }

    return data;
}

data_t*
lstn_port(const char* port_str, int* lstng_sock, int* epoll_fd) {
    *lstng_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*lstng_sock < 0) {
        printf("Error occured while creating socket.\n");
    }
    struct sockaddr_in serv_sock_addr;
    serv_sock_addr.sin_family = AF_INET;
    uint16_t port;
    sscanf(port_str, "%" SCNd16 "", &port);
    serv_sock_addr.sin_port = htons(port);
    serv_sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // localhost

    if (bind(*lstng_sock, (struct sockaddr*)&serv_sock_addr, sizeof(serv_sock_addr)) < 0) {
        printf("Error occured while binding to the port.\n");
        exit(1);
    }

    if (listen(*lstng_sock, SOMAXCONN) < 0) {
        printf("Error occured while starting listening the socket.\n");
        exit(1);
    }

    if ((*epoll_fd = epoll_create1(0)) < 0) {
        exit(1);
    }

    return create_event_data(*epoll_fd, *lstng_sock);
}

data_t*
process_lstn_sock(int epoll_fd, int lstng_sock, struct epoll_event* event) {
    int clnt_sock = accept(lstng_sock, NULL, NULL);
    if (clnt_sock < 0) {
        printf("Error occured while accepting client.\n");
        return NULL;
    }
    return create_event_data(epoll_fd, clnt_sock);
}

int main(int argc, char** argv) {
    struct sigaction sig_term_struct;

    sig_term_struct.sa_handler = signal_handler;
    sigemptyset(&sig_term_struct.sa_mask);
    sig_term_struct.sa_flags = 0;

    sigaction(SIGTERM, &sig_term_struct, NULL);

    if (argc != 2) {
        printf("There must be 1 argument only.\n");
        return 1;
    }
    int listenning_sock;
    int epoll_fd;
    data_t* listen_event = lstn_port(argv[1], &listenning_sock, &epoll_fd);
    int conn_num = 0;
    data_t* conn[MAX_CONNECTIONS];

    struct epoll_event pending[MAX_EVENTS];
    // printf("waiting client...\n");
    while (!end) {
        int ev_num = epoll_wait(epoll_fd, pending, MAX_EVENTS, -1);
        for (int i = 0; i < ev_num; ++i) {
            data_t* tmp_data = (data_t*)(pending[i].data.ptr);
            if (tmp_data->fd == listenning_sock) {
                data_t* new_conn = process_lstn_sock(epoll_fd, listenning_sock, &(pending[i]));
                if (new_conn != NULL) {
                    if (conn_num == MAX_CONNECTIONS) {
                        printf("Maximum number of connections!\n");
                        shutdown_clnt(new_conn->fd);
                        continue;
                    }
                    conn[conn_num] = new_conn;
                    conn_num++;
                    // printf("client connected...\n");
                }
            } else {
                process_clnt(&(pending[i]));
                // printf("request proceed...\n");
            }
        }
    }
    // close all connections
    close(epoll_fd);
    close(listenning_sock);
    for (int i = 0; i < conn_num; ++i) {
        if (!conn[i]->closed) {
            shutdown_clnt(conn[i]->fd);
        }
        free(conn[i]);
    }
}










