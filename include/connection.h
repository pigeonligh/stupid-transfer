#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <stdint.h>
#include <arpa/inet.h>

struct connection_info
{
    int32_t fd;

    uint32_t secs;
    uint32_t count;
};

void init_connections();

connection_info *first_connection();

connection_info *next_connection();

void new_connection(int fd);

void close_connection(int32_t fd);

connection_info *find_connection(int32_t fd);

void close_all_connections();

#endif /* _CONNECTION_H_ */
