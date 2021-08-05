/*
Copyright 2020 The FileTransfer Authors.

Licensed under the GNU General Public License, v3.0

See LICENSE in the project root for license information.
*/

#ifndef _PACKET_H_
#define _PACKET_H_

#include <stdint.h>

#define PACKET_HEADER_SIZE 5
#define MAX_PACKET_SIZE 4096
#define HASH_SIZE 256

#define TYPE_CONNECT 100
#define TYPE_CONNECTED 101
#define TYPE_REQUEST 102
#define TYPE_RESPONSE 103
#define TYPE_SEND 104
#define TYPE_SAY 105

#define KEEPALIVE 110

#define REQUEST_LS 200
#define REQUEST_CD 201
#define REQUEST_RM 202
#define REQUEST_PWD 203
#define REQUEST_MKDIR 204
#define REQUEST_RMDIR 205

#define REQUEST_UPLOAD 210
#define REQUEST_DOWNLOAD 211

#define SEND_CONTINUE 220
#define SEND_REPEAT 221
#define SEND_DONE 222

#define STATUS_SUCCEED 300
#define STATUS_FAILED 301

struct packet {
    uint32_t length;
    uint8_t type;

    uint8_t data[MAX_PACKET_SIZE];
};

struct packet_data {
    uint32_t option;
    uint8_t data[MAX_PACKET_SIZE - 4];
};

struct send_data {
    uint8_t data[MAX_PACKET_SIZE - HASH_SIZE];
    uint8_t hash[HASH_SIZE - 4];
    uint32_t length;
};

bool receive_packet(int32_t fd, packet *pack);

void gen_hash(send_data *data);

bool check_hash(send_data *data);

#endif /* _PACKET_H */
