/*
Copyright 2020 The FileTransfer Authors.

Licensed under the GNU General Public License, v3.0

See LICENSE in the project root for license information.
*/

#ifndef _LOCK_H_
#define _LOCK_H_

#include <pthread.h>

extern pthread_mutex_t mutex;

void set_lock();
void unset_lock();

#endif /* _LOCK_H_ */