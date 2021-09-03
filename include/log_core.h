#ifndef _LOG_CORE_H_
#define _LOG_CORE_H_

#include <time.h>
#include "log_list.h"
#include "log_lock.h"
#include "log_thread.h"

typedef struct fileNode
{
    stListEntry entry;
    char fileName[1024];
}stFileNode;

typedef struct logCore
{
    stListHead list[2];
    stListHead *read_list;
    stListHead *write_list;
    SPIN_LOCK_T list_lock;
    THREAD_T thread;
    int isAlive;
    unsigned long lastTime;
    struct tm lasttm;
    unsigned int existfiles;
    stListHead file_list;
}stLogCore;

void log_core_init();
void log_core_destroy();

#endif
