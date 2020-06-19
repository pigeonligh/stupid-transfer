#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <stdint.h>

bool client_start(const char* ipaddr, int32_t port);
void client_stop();

bool is_waiting();

#endif /* _CLIENT_H_ */
