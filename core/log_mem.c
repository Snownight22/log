#include <stdio.h>
#include <stdlib.h>
#include "log.h"
#include "log_list.h"
#include "log_lock.h"
#include "log_mem.h"

stListHead g_log_mem_head;
stLogUnit* g_log_unit_array;
SPIN_LOCK_T g_log_mem_lock;

void* log_mem_malloc()
{
    if (NULL == g_log_mem_head.first)
    {
        fprintf(stderr, "log mem malloc error\n");
        return NULL;
    }

    stLogUnit *node = container_of(g_log_mem_head.first, stLogUnit, entry);
    SPIN_LOCK(&g_log_mem_lock);
    LIST_DELETE(&g_log_mem_head, &(node->entry));
    SPIN_UNLOCK(&g_log_mem_lock);

    return node;
}

void log_mem_free(stLogUnit *node)
{
    SPIN_LOCK(&g_log_mem_lock);
    LIST_INSERT_TAIL(&g_log_mem_head, &(node->entry));
    SPIN_UNLOCK(&g_log_mem_lock);
}

int log_mem_init(int node_num)
{
    int i;
    stListHead *listHead = &g_log_mem_head;

    g_log_unit_array = (stLogUnit *) malloc(sizeof(stLogUnit) * node_num);
    if (NULL == g_log_unit_array)
    {
        fprintf(stderr, "log memory init error\n");
        return LOG_ERR_FAIL;
    }

    LIST_INIT(listHead);
    for (i = 0; i < node_num; i++)
    {
        LIST_INSERT(listHead, &g_log_unit_array[i].entry);
    }

    SPIN_LOCK_INIT(&g_log_mem_lock);

    return LOG_ERR_OK;
}


void log_mem_destory()
{
    if (NULL != g_log_unit_array)
        free(g_log_unit_array);
    g_log_unit_array = NULL;

    LIST_INIT(&g_log_mem_head);
    SPIN_LOCK_DESTROY(&g_log_mem_lock);
}


