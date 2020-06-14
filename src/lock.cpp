#include "lock.h"

pthread_mutex_t mutex;

void set_lock() {
    pthread_mutex_lock(&mutex);
}

void unset_lock() {
    pthread_mutex_unlock(&mutex);
}
