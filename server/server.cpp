/*
Copyright 2020 The FileTransfer Authors.

Licensed under the GNU General Public License, v3.0

See LICENSE in the project root for license information.
*/

#include "server.h"

#include "connection_info.h"
#include "connection_core.h"
#include "keepalive.h"
#include "packet.h"
#include "event.h"
#include "lock.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>

void new_connection(const epoll_event &event);
void listen_from_connection(const epoll_event &event);

int32_t listen_fd = -1;

int32_t init_fd(int32_t port) {
    int32_t fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    if (fd == -1) {
        perror("create socket");
        exit(EXIT_FAILURE);
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr = {INADDR_ANY};
    server_addr.sin_port = htons(port);
    if (bind(fd, (sockaddr*) &server_addr, sizeof server_addr) == -1) {
        perror("bind socket");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (listen(fd, MAX_CONNECTIONS) == -1) {
        perror("listen socket");
        close(fd);
        exit(EXIT_FAILURE);
    }

    add_event(fd);

    return fd;
}

void init(int32_t port) {
    init_event();

    listen_fd = init_fd(port);
    printf("server runs in *:%d\n", port);

    init_connections();
    keepalive_start();
}

void run_once() {
    epoll_event event;
    read_event(&event);
    printf("get event\n");
    if (event.data.fd == listen_fd) {
        new_connection(event);
    } else {
        listen_from_connection(event);
    }
}

void server_start(const char *dir, int32_t port) {
    init(port);
    set_root(dir);
    while (true) {
        run_once();
    } 
}

void server_stop() {
    keepalive_close();
    close_event();
    close_all_connections();
    close(listen_fd);
}

void new_connection(const epoll_event &event) {
    int32_t fd = event.data.fd;
    printf("create connection from %d\n", fd);

    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int32_t connect_fd = accept(fd, (sockaddr*) &client_addr, &addr_len);

    if (connect_fd == -1) {
        printf("create connection failed.");
        return;
    }

    set_lock();

    connection_info *ci = new_connection(connect_fd);
    add_event(ci->fd);

    unset_lock();
}

void process_packet(connection_info* ci) {
    int32_t client_fd = ci->fd;

    packet pack;
    if (receive_packet(client_fd, &pack) == false) {
        printf("client %d disconnected\n", ci->fd);
        close_connection(client_fd);
        return;
    }

    if (pack.type == TYPE_CONNECT) {
        // this is useless
    } else if (pack.type == TYPE_REQUEST) {
        packet_data *data = (packet_data*) pack.data;
        ci->deal_request(data);
    } else if (pack.type == TYPE_SEND) {
        send_data *data = (send_data*) pack.data;
        ci->deal_send(data);
    } else if (pack.type == TYPE_SAY) {
        for (connection_info *rc = first_connection(); rc != nullptr; rc = next_connection()) {
            if (rc->fd == client_fd) {
                continue;
            }
            rc->send_packet(&pack);
        }
    } else if (pack.type == KEEPALIVE) {
        ci->secs = time(nullptr);
    } else {
        printf("unknown type packet\n");
    }
}

void listen_from_connection(const epoll_event &event) {
    if (event.events & EPOLLIN) {
        int32_t fd = event.data.fd;
        printf("listen from %d\n", fd);
        connection_info *ci = find_connection(fd);
        if (ci != nullptr) {
            process_packet(ci);
        }
    }
}
