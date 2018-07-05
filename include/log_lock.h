#ifndef _LOG_LOCK_H_
#define _LOG_LOCK_H_

#include <pthread.h>

#define SPIN_LOCK_T pthread_mutex_t

#define SPIN_LOCK_INIT(lock) pthread_mutex_init(lock, NULL)

#define SPIN_LOCK_DESTROY(lock) pthread_mutex_destroy(lock)

#define SPIN_LOCK(lock) pthread_mutex_lock(lock)

#define SPIN_UNLOCK(lock) pthread_mutex_unlock(lock)

#define SPIN_TRYLOCK(lock) pthread_mutex_trylock(lock)

#endif
