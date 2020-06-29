/*
Copyright 2020 The FileTransfer Authors.

Licensed under the GNU General Public License, v3.0

See LICENSE in the project root for license information.
*/

#ifndef _KEEPALIVE_H_
#define _KEEPALIVE_H_

#define DEAD_TIME 60
#define ALIVE_TIME 20

void keepalive_start();
void keepalive_close();

#endif /* _KEEPALIVE_H_ */
