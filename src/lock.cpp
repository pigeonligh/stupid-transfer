/*
Copyright 2020 The FileTransfer Authors.

Licensed under the GNU General Public License, v3.0

See LICENSE in the project root for license information.
*/

#include "lock.h"

#include <pthread.h>

pthread_mutex_t mutex;

void set_lock() {
    pthread_mutex_lock(&mutex);
}

void unset_lock() {
    pthread_mutex_unlock(&mutex);
}
