#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <stdint.h>

#define STATUS_READY 0
#define STATUS_TRANSFERING 1

bool client_start(const char* ipaddr, int32_t port);
void client_stop();

void set_status(int status);
bool is_waiting();

void send_packet(struct packet *pack);

#endif /* _CLIENT_H_ */
