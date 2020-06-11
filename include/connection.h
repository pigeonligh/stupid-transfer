#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <stdint.h>
#include <arpa/inet.h>

struct connection_info {
    int32_t fd;

    uint32_t secs;
    uint32_t count;
};

void init_connections();

// TODO: first_connection
// TODO: next_connection
// TODO: new_connection
// TODO: close_connection

connection_info* find_connection(int32_t fd);

void close_all_connections();

#endif /* _CONNECTION_H_ */
