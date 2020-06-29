/*
Copyright 2020 The FileTransfer Authors.

Licensed under the GNU General Public License, v3.0

See LICENSE in the project root for license information.
*/

#include "event.h"

#include <sys/epoll.h>

#include <unistd.h>
#include <stdlib.h>

epoll_event event;
epoll_event event_buff[EVENT_BUFF_SIZE];

int buff_size = 0;
int buff_now = 0;

int32_t epoll_fd = -1;

void init_event() {
    epoll_fd = epoll_create(MAX_EPOLL_EVENT);
}

void close_event() {
    close(epoll_fd);
}

void add_event(int32_t event_fd) {
    event.data.fd = event_fd;
    event.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &event);
}

void del_event(int32_t event_fd) {
    event.data.fd = event_fd;
    event.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event);
}

void read_event(epoll_event* event) {
    while (buff_now == buff_size) {
        buff_size = epoll_wait(epoll_fd, event_buff, EVENT_BUFF_SIZE, EVENT_READ_TIMEOUT);
        buff_now = 0;
    }
    *event = event_buff[buff_now++];
}
