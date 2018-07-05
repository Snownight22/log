#ifndef _LOG_THREAD_H_
#define _LOG_THREAD_H_

#include <pthread.h>

#define THREAD_T pthread_t

#define THREAD_CREATE(thread, attr, func, arg) \
    pthread_create((thread), (attr), (func), (arg))

#define THREAD_JOIN(thread, retval) \
    pthread_join((thread), (retval))

#endif
