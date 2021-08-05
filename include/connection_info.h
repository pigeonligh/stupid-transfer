/*
Copyright 2020 The FileTransfer Authors.

Licensed under the GNU General Public License, v3.0

See LICENSE in the project root for license information.
*/

#ifndef _CONNECTION_INFO_H_
#define _CONNECTION_INFO_H_

#include <stdint.h>

struct connection_info {
    int32_t fd;

    int32_t secs;
    int32_t count;

    struct connection_core* core;

    void send_packet(struct packet *pack);
    void deal_request(struct packet_data *);
    void deal_send(struct send_data *);
};

void init_connections();

connection_info* first_connection();

connection_info* next_connection();

connection_info* new_connection(int fd);

bool close_connection(int32_t fd);

connection_info* find_connection(int32_t fd);

void close_all_connections();

#endif /* _CONNECTION_INFO_H_ */
