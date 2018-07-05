#ifndef _LOG_MEM_H_
#define _LOG_MEM_H_
#include "log_list.h"

#define LOG_UNIT_NODE_MAX    (2048)
#define LOG_CONTENT_LENGTH_MAX    (1024)

typedef struct log_unit
{
    stListEntry entry;
    unsigned long nowtime;
    unsigned int length;
    char content[LOG_CONTENT_LENGTH_MAX];
}stLogUnit;

void* log_mem_malloc();
void log_mem_free(stLogUnit *node);
int log_mem_init(int node_num);
void log_mem_destory();

#endif
