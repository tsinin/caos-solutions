#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
    int fd;
    size_t count;
    int done;
} data_t;

size_t count_and_close(int epoll_fd, data_t** entries, int N) {
    close(epoll_fd);
    size_t summary_bytes = 0;
    for (size_t i = 0; i < N; ++i) {
        summary_bytes += entries[i]->count;
        close(entries[i]->fd);
        free(entries[i]);
    }
    return summary_bytes;
}

void
process_event(struct epoll_event *event)
{
    data_t* data_ptr = (data_t*)(event->data.ptr);
    if (event->events & EPOLLIN) {
        ssize_t readed;
        char buff[4096];
        while ((readed = read(data_ptr->fd, buff, 4096)) > 0) {
            data_ptr->count += readed;
        }
        if (readed == 0) {
            data_ptr->done = 1;
            close(data_ptr->fd);
        }
    } else if (event->events & EPOLLHUP) {
        data_ptr->done = -1;
        close(data_ptr->fd);
    }
}

data_t*
create_event_data(int epoll_fd, int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
    data_t *data = malloc(sizeof(*data));
    data->fd = fd;
    data->done = 0;
    data->count = 0;

    struct epoll_event event_ready_read;
    event_ready_read.events = EPOLLIN;
    event_ready_read.data.ptr = data;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event_ready_read);

    return data;
}

size_t
read_data_and_count(size_t N, int in[N]) {
    int epoll_fd;
    if ((epoll_fd = epoll_create1(0)) < 0) {
        exit(1);
    }
    data_t **entries = malloc(N * sizeof(*entries));

    for (size_t i = 0; i < N; ++i) {
        entries[i] = create_event_data(epoll_fd, in[i]);
    }

    size_t files_left = N;
    struct epoll_event pending[65536];
    while (files_left) {
        int events = epoll_wait(epoll_fd, pending, 65536, -1);
        for (int i = 0; i < events; ++i) {
            process_event(&(pending[i]));
            if (((data_t*)(pending[i].data.ptr))->done == 1) {
                files_left--;
            } else if (((data_t*)(pending[i].data.ptr))->done == -1) {
                return count_and_close(epoll_fd, entries, N);
            }
        }
    }
    return count_and_close(epoll_fd, entries, N);
}
