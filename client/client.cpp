/*
Copyright 2020 The FileTransfer Authors.

Licensed under the GNU General Public License, v3.0

See LICENSE in the project root for license information.
*/

#include "client.h"

#include "event.h"
#include "lock.h"
#include "packet.h"
#include "mails.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int32_t sock_fd = -1;

pthread_t client_thread;

uint8_t now_status;
uint8_t now_command;

std::string file_name;
FILE* file_fd;

bool process_packet();

void deal_response(packet_data *, int32_t);
void deal_send(send_data *);
bool open_file(const std::string &, const char *);

uint32_t getData(bool, uint8_t *);
bool setData(uint8_t *);

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
        deal_response(data, pack.length);
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

void set_command(int command) {
    now_command = command;
}

bool is_waiting() {
    return now_status != STATUS_READY;
}

void send_packet(packet *pack) {
    int length = pack->length;
    send(sock_fd, pack, length, 0);
}


void deal_response(packet_data *data, int32_t length) {
    if (now_status == STATUS_READY) {
        return;
    }
    if (data->option == STATUS_SUCCEED) {
        if (now_command == REQUEST_LS) {
            packet pack;
            pack.length = PACKET_HEADER_SIZE + 4;
            pack.type = TYPE_REQUEST;
            packet_data *pdata = (packet_data*) pack.data;
            pdata->option = SEND_CONTINUE;
            send_packet(&pack);
        } else if (now_command == REQUEST_CD) {
            set_status(STATUS_READY);
        } else if (now_command == REQUEST_RM) {
            set_status(STATUS_READY);
        } else if (now_command == REQUEST_PWD) {
            data->data[length - PACKET_HEADER_SIZE - 4] = 0;
            printf("%s\n", (char*) data->data);
            set_status(STATUS_READY);
        } else if (now_command == REQUEST_RMDIR) {
            set_status(STATUS_READY);
        } else if (now_command == REQUEST_MKDIR) {
            set_status(STATUS_READY);
        } else if (now_command == REQUEST_UPLOAD) {
            if (open_file(file_name.c_str(), "rb")) {
                packet pack;
                pack.length = PACKET_HEADER_SIZE + 4;
                pack.type = TYPE_REQUEST;
                packet_data *pdata = (packet_data*) pack.data;
                pdata->option = STATUS_SUCCEED;
                send_packet(&pack);
            } else {
                packet pack;
                pack.length = PACKET_HEADER_SIZE + 4;
                pack.type = TYPE_REQUEST;
                packet_data *pdata = (packet_data*) pack.data;
                pdata->option = STATUS_FAILED;
                send_packet(&pack);
                set_status(STATUS_READY);
            }
        } else if (now_command == REQUEST_DOWNLOAD) {
            if (open_file(file_name.c_str(), "wb")) {
                packet pack;
                pack.length = PACKET_HEADER_SIZE + 4;
                pack.type = TYPE_REQUEST;
                packet_data *pdata = (packet_data*) pack.data;
                pdata->option = SEND_CONTINUE;
                send_packet(&pack);
            } else {
                packet pack;
                pack.length = PACKET_HEADER_SIZE + 4;
                pack.type = TYPE_REQUEST;
                packet_data *pdata = (packet_data*) pack.data;
                pdata->option = STATUS_FAILED;
                send_packet(&pack);
                set_status(STATUS_READY);
            }
        } else {
            fprintf(stderr, "something is wrong.1\n");
            set_status(STATUS_READY);
        } 
    } else if (data->option == SEND_CONTINUE) {
        packet pack;
        int32_t length = getData(true, pack.data);
        if (length >= 0) {
            pack.length = PACKET_HEADER_SIZE + length;
            pack.type = TYPE_SEND;
        } else {
            pack.length = PACKET_HEADER_SIZE + 4;
            pack.type = TYPE_REQUEST;
            packet_data *pdata = (packet_data*) pack.data;
            pdata->option = STATUS_FAILED;
            set_status(STATUS_READY);
        }
        send_packet(&pack);
    } else if (data->option == SEND_REPEAT) {
        packet pack;
        int32_t length = getData(false, pack.data);
        if (length >= 0) {
            pack.length = PACKET_HEADER_SIZE + length;
            pack.type = TYPE_SEND;
        } else {
            pack.length = PACKET_HEADER_SIZE + 4;
            pack.type = TYPE_REQUEST;
            packet_data *pdata = (packet_data*) pack.data;
            pdata->option = STATUS_FAILED;
            set_status(STATUS_READY);
        }
        send_packet(&pack);
    } else if (data->option == SEND_DONE) {
        if (file_fd != nullptr) {
            fclose(file_fd);
            file_fd = nullptr;
        }
        set_status(STATUS_READY);
    } else {
        fprintf(stderr, "something is wrong. (Error %d)\n", data->option);
        set_status(STATUS_READY);
    }
}

void deal_send(send_data *data) {
    if (now_status != STATUS_TRANSFERING) {
        return;
    }
    packet pack;
    memset(&pack, 0, sizeof pack);
    packet_data *pdata = (packet_data*) pack.data;
    pack.type = TYPE_REQUEST;
    if (check_hash(data)) {
        if (setData(data->data)) {
            pdata->option = SEND_CONTINUE;
        } else {
            pdata->option = STATUS_FAILED;
        }
    } else {
        pdata->option = SEND_REPEAT;
    }
    pack.length = PACKET_HEADER_SIZE + 4;
    send_packet(&pack);
}

void set_filename(const std::string &filename) {
    file_name = filename;
}

bool open_file(const std::string &file, const char *mode) {
    if (file_fd != nullptr) {
        fclose(file_fd);
    }
    file_fd = fopen(file.c_str(), mode);
    return file_fd != nullptr;
}

int32_t length;
uint8_t buff[MAX_PACKET_SIZE];

bool prepareData() {
    if (now_command == REQUEST_UPLOAD) {
        if (file_fd == nullptr) {
            return false;
        }
        send_data _data;
        memset(&_data, 0, sizeof _data);
        length = fread(_data.data, MAX_PACKET_SIZE - HASH_SIZE, 1, file_fd);
        gen_hash(&_data);
        memcpy(buff, &_data, MAX_PACKET_SIZE);
    } else {
        return false;
    }
    return true;
}

uint32_t getData(bool checked, uint8_t *data) {
    if (checked) {
        if (prepareData() == false) {
            return -1;
        }
    }
    if (length > 0) {
        memcpy(data, buff, MAX_PACKET_SIZE);
        return MAX_PACKET_SIZE;
    }
    return 0;
}

bool setData(uint8_t *data) {
    if (now_command == REQUEST_DOWNLOAD) {
        if (file_fd == nullptr) {
            return false;
        }
        int _length = 0;
        while (*(data + _length) != '\0' && _length < MAX_PACKET_SIZE - HASH_SIZE)
            ++ _length;
        fwrite(data, _length, 1, file_fd);
    } else if (now_command == REQUEST_LS) {
        std::string dirs((char*) data);
        printf("%s", dirs.c_str());
    } else {
        return false;
    }
    return true;
}
