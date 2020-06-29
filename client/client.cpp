#include "client.h"

#include "event.h"
#include "lock.h"
#include "packet.h"
#include "mails.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int32_t sock_fd = -1;

pthread_t client_thread;

uint8_t now_status;
int32_t file_fd;

bool process_packet();

int32_t connect_server(const char* ipaddr, int32_t port) {
    int32_t fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1) {
        perror("create socket");
        return -1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ipaddr);
    server_addr.sin_port = htons(port);
    if (connect(fd, (sockaddr*) &server_addr, sizeof server_addr) == -1) {
        perror("bind socket");
        close(fd);
        return -1;
    }

    add_event(fd);

    return fd;
}

void* client_receiver(void *obj) {
    while (true) {
        epoll_event event;
        read_event(&event);
        if (event.data.fd == sock_fd) {
            set_lock();
            if (process_packet() == false) {
                unset_lock();
                break;
            }
            unset_lock();
        }
    }
    return nullptr;
}

bool client_start(const char* ipaddr, int32_t port) {
    init_event();
    sock_fd = connect_server(ipaddr, port);

    if (sock_fd == -1) {
        close_event();
        return false;
    }

    pthread_create(&client_thread, nullptr, client_receiver, nullptr);
    now_status = STATUS_READY;
    return true;
}

void client_stop() {
    pthread_cancel(client_thread);
    close_event();
    close(sock_fd);
}

bool process_packet() {
    packet pack;
    if (receive_packet(sock_fd, &pack) == false) {
        printf("pipe broken\n");
        return false;
    }

    if (pack.type == TYPE_CONNECTED) {
        // this is useless
    } else if (pack.type == TYPE_RESPONSE) {
        packet_data *data = (packet_data*) pack.data;
        deal_response(data);
    } else if (pack.type == TYPE_SEND) {
        send_data *data = (send_data*) pack.data;
        deal_send(data);
    } else if (pack.type == TYPE_SAY) {
        append_mail(std::string((char*) pack.data));
    } else if (pack.type == KEEPALIVE) {
        send_packet(&pack);
    } else {
        printf("unknown type packet\n");
    }

    return true;
}

void set_status(int status) {
    now_status = status;
}

bool is_waiting() {
    return now_status != STATUS_READY;
}

void send_packet(packet *pack) {
    int length = pack->length;
    send(sock_fd, pack, length, 0);
}


void deal_response(packet_data *data) {
    // TODO
}

void deal_send(send_data *data) {
    // TODO
}