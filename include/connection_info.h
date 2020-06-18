#ifndef _CONNECTION_INFO_H_
#define _CONNECTION_INFO_H_

#include <stdint.h>

struct connection_info {
    int32_t fd;

    uint32_t secs;
    uint32_t count;
};

void init_connections();

connection_info* first_connection();

connection_info* next_connection();

connection_info* new_connection(int fd);

bool close_connection(int32_t fd);

connection_info* find_connection(int32_t fd);

void close_all_connections();

#endif /* _CONNECTION_INFO_H_ */
