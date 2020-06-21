#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdint.h>

#define DEFAULT_SERVER_PORT 10082
#define MAX_CONNECTIONS 128

void server_start(const char *dir, int32_t);
void server_stop();

void server_test();

#endif /* _SERVER_H_ */
