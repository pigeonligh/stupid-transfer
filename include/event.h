/*
Copyright 2020 The FileTransfer Authors.

Licensed under the GNU General Public License, v3.0

See LICENSE in the project root for license information.
*/

#ifndef _EVENT_H_
#define _EVENT_H_

#include <stdint.h>

#define MAX_EPOLL_EVENT 128
#define EVENT_BUFF_SIZE 32
#define EVENT_READ_TIMEOUT 1000

void init_event();
void close_event();

void add_event(int32_t event_fd);
void del_event(int32_t event_fd);

void read_event(struct epoll_event* event);

#endif /* _EVENT_H_ */
