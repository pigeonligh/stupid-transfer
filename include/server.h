#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdint.h>

#define DEFAULT_SERVER_PORT 10082
#define MAX_CONNECTIONS 128

void server_start(int32_t);
void server_close();

#endif /* _SERVER_H_ */
