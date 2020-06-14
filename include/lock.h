#ifndef _LOCK_H_
#define _LOCK_H_

#include <pthread.h>

extern pthread_mutex_t mutex;

void set_lock();
void unset_lock();

#endif /* _LOCK_H_ */