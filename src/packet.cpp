#include "packet.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

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