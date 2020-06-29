/*
Copyright 2020 The FileTransfer Authors.

Licensed under the GNU General Public License, v3.0

See LICENSE in the project root for license information.
*/

#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdint.h>

#define DEFAULT_SERVER_PORT 10082
#define MAX_CONNECTIONS 128

void server_start(const char *dir, int32_t);
void server_stop();

void server_test(const char *dir, int32_t);

#endif /* _SERVER_H_ */
