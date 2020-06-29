#include "packet.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "md5/md5.h"

bool receive_packet(int32_t fd, packet *pack) {
    int32_t len = 0;

    while (len < PACKET_HEADER_SIZE) {
        len += recv(fd, pack + len, PACKET_HEADER_SIZE - len, 0);
        if (len <= 0) {
            return false;
        }
    }

    if (pack->length > sizeof(packet)) {
        return false;
    }

    int data_size = pack->length - PACKET_HEADER_SIZE;
    len = 0;
    while (len < data_size) {
        len += recv(fd, pack->data + len, data_size - len, 0);
        if (len < 0) {
            return false;
        }
    }

    return true;
}

void gen_hash(send_data *data) {
    char src[MAX_PACKET_SIZE - HASH_SIZE];
    memcpy(src, data->data, sizeof (data->data));
    std::string value = md5(src, MAX_PACKET_SIZE - HASH_SIZE);
    std::string hash = value;
    for (int i = 1; i < 8; ++ i) {
        value = value.substr(value.size() - 4, 4) + value.substr(0, value.size() - 4);
        hash += value;
    }
    memcpy(data->hash, hash.c_str(), sizeof (data->hash));
}

bool check_hash(send_data *data) {
    char src[MAX_PACKET_SIZE - HASH_SIZE];
    memcpy(src, data->data, sizeof (data->data));
    std::string value = md5(src, MAX_PACKET_SIZE - HASH_SIZE);
    std::string hash = value;
    for (int i = 1; i < 8; ++ i) {
        value = value.substr(value.size() - 4, 4) + value.substr(0, value.size() - 4);
        hash += value;
    }
    char hash_value[256];
    memcpy(hash_value, data->hash, sizeof (data->hash));
    return strcmp(hash.c_str(), hash_value) == 0;
}