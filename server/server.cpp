#include "server.h"
#include "connection.h"
#include "keepalive.h"
#include "event.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>

int32_t listen_fd = -1;

int32_t init_fd(int32_t port) {
    int32_t fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
        // TODO: new connection
    } else {
        // TODO: listen from connected client
    }
}

void server_start(int32_t port) {
    init(port);
    while (true) {
        run_once();
    } 
}

void server_close() {
    close(listen_fd);
    keepalive_close();
    close_event();
    close_all_connections();
}
