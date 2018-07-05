#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include "log.h"
#include "log_config.h"
#include "log_thread.h"
#include "log_mem.h"
#include "log_core.h"

stLogCore g_log_core_handler;

stLogUnit* log_core_node_get()
{
    stLogCore *handler = &g_log_core_handler;
    stLogUnit *node = NULL;
    stListEntry *entry = handler->read_list->first;
    if (NULL == entry)
    {
        if (0 == SPIN_TRYLOCK(&(handler->list_lock)))
        {
            stListHead *tmp = handler->read_list;
            handler->read_list = handler->write_list;
            handler->write_list = tmp;
            SPIN_UNLOCK(&(handler->list_lock));
        }
    }

    entry = handler->read_list->first;
    if (NULL != entry)
    {
        LIST_DELETE(handler->read_list, entry);
        node = container_of(entry, stLogUnit, entry); 
    }

    return node;
}

void log_core_node_put(stLogUnit *node)
{
    stLogCore *handler = &g_log_core_handler;
    SPIN_LOCK(&(handler->list_lock));
    LIST_INSERT_TAIL(handler->write_list, &(node->entry));
    SPIN_UNLOCK(&(handler->list_lock));
}

void log_core_printf(eLogLevel level, const char* format, ...)
{
    stLogConfig *confighandler = log_config_handler_get();
    va_list ap;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm *nowTime;
    nowTime = localtime(&tv.tv_sec);
    char fmt[LOG_CONTENT_LENGTH_MAX] = {0};

    snprintf(fmt, LOG_CONTENT_LENGTH_MAX-1, "[%04d-%02d-%02d %02d:%02d:%02d:%03d]%s", nowTime->tm_year+1900, \
            nowTime->tm_mon+1, nowTime->tm_mday, nowTime->tm_hour, nowTime->tm_min, nowTime->tm_sec, tv.tv_usec/1000, format);

    if (level >= confighandler->log_level)
    {
        if (confighandler->log_console)
        {
            va_start(ap, format);
            vprintf(fmt, ap);
            va_end(ap);
        }

        if (confighandler->log_file)
        {
            stLogUnit *node = log_mem_malloc();
            if (NULL != node)
            {
                node->nowtime = (unsigned long)tv.tv_sec;
                va_start(ap, format);
                node->length = vsnprintf(node->content, LOG_CONTENT_LENGTH_MAX - 1, fmt, ap);
                va_end(ap);
                log_core_node_put(node);
            }
        }
    }
}

void log_core_file_oper(char *fileName)
{
    stFileNode *fileNode;
    stLogConfig *configHandler = log_config_handler_get();
    stLogCore *handler = &g_log_core_handler;

    if (NULL == (fileNode = (stFileNode*) malloc (sizeof(stFileNode))))
    {
        fprintf(stderr, "file oper:malloc error\n");
        return ;
    }

    strncpy(fileNode->fileName, fileName, 1023);
    fileNode->fileName[1024] = 0;
    LIST_INSERT_TAIL(&(handler->file_list), &(fileNode->entry));
    handler->existfiles++;

    while(handler->existfiles > configHandler->rolling_filenums)
    {
        stListEntry *entry = handler->file_list.first;
        LIST_DELETE(&(handler->file_list), entry);
        handler->existfiles--;
        fileNode = container_of(entry, stFileNode, entry);
        remove(fileNode->fileName);
        free(fileNode);
    }
}

FILE* log_core_rolling(FILE *fp, unsigned long logtime)
{
    stLogCore *handler = &g_log_core_handler;
    stLogConfig *confighandler = log_config_handler_get();
    char filename[CONFIG_FILE_LENGTH_MAX] = {0};
    char cmd[2*CONFIG_FILE_LENGTH_MAX] = {0};

    if (0 == handler->lastTime)
    {
        handler->lastTime = logtime;
        struct tm *ntm = localtime(&logtime);
        memcpy((void*)&handler->lasttm, (void*)ntm, sizeof(struct tm));
        return fp;
    }

    if (confighandler->rolling_filenums != 0)
    {
        struct tm *lasttm = &handler->lasttm;    //localtime(&handler->lastTime);
        struct tm *logtm = localtime(&logtime);
        if (NULL != fp)
        {
            if (confighandler->rolling_day)
            {
                if ((logtm->tm_mday != lasttm->tm_mday) || (logtm->tm_mday == lasttm->tm_mday && handler->lastTime + 24*60*60 < logtime))
                {
                    fclose(fp);
                    fp = NULL;
                    snprintf(filename, CONFIG_FILE_LENGTH_MAX-1, "%04d-%02d-%02d_%s", lasttm->tm_year+1900, \
                            lasttm->tm_mon+1, lasttm->tm_mday, confighandler->log_filename);
                    rename(confighandler->log_filename, filename);
                    handler->lastTime = logtime;
                    memcpy((void*)&handler->lasttm, (void*)logtm, sizeof(struct tm));
                    log_core_file_oper(filename);
                }
            }
            else if (confighandler->rolling_hour)
            {
                if ((logtm->tm_hour != lasttm->tm_hour) || (logtm->tm_hour == lasttm->tm_hour && handler->lastTime + 60*60 <= logtime))
                {
                    fclose(fp);
                    fp = NULL;
                    snprintf(filename, CONFIG_FILE_LENGTH_MAX-1, "%04d-%02d-%02d-%02d_%s", lasttm->tm_year+1900, \
                            lasttm->tm_mon+1, lasttm->tm_mday, lasttm->tm_hour, confighandler->log_filename);
                    rename(confighandler->log_filename, filename);
                    handler->lastTime = logtime;
                    memcpy((void*)&handler->lasttm, (void*)logtm, sizeof(struct tm));
                    log_core_file_oper(filename);
                }
            }
            else if (confighandler->rolling_min)
            {
                if ((logtm->tm_min != lasttm->tm_min) || (logtm->tm_min == lasttm->tm_min && handler->lastTime + 60 <= logtime))
                {
                    fclose(fp);
                    fp = NULL;
                    snprintf(filename, CONFIG_FILE_LENGTH_MAX-1, "%04d-%02d-%02d-%02d-%02d_%s", lasttm->tm_year+1900, \
                            lasttm->tm_mon+1, lasttm->tm_mday, lasttm->tm_hour, lasttm->tm_min, confighandler->log_filename);
                    rename(confighandler->log_filename, filename);
                    handler->lastTime = logtime;
                    memcpy((void*)&handler->lasttm, (void*)logtm, sizeof(struct tm));
                    log_core_file_oper(filename);
                }
            }

            if (NULL == fp)
                fp = fopen(confighandler->log_filename, "ab+");

            return fp;
        }
    }

    return fp;
}

void* log_core_process(void *arg)
{
    stLogCore *handler = &g_log_core_handler;
    stLogConfig *config_handler = log_config_handler_get();
    FILE *fp = NULL;
    stLogUnit *node = NULL;

    while (handler->isAlive)
    {
        node = log_core_node_get();
        if (NULL == node)
        {
            sleep(1);
            continue;
        }

        fp = log_core_rolling(fp, node->nowtime);
        if (NULL == fp)
        {
            if (NULL == (fp = fopen(config_handler->log_filename, "ab+")))
            {
                fprintf(stderr, "log file open error\n");
                if (NULL != node)
                    log_mem_free(node);
                continue;
            }
        }

        fwrite(node->content, node->length, 1, fp);
        log_mem_free(node);
        fflush(fp);
    }

    if (NULL != fp)
        fclose(fp);
}

void log_core_init()
{
    log_mem_init(LOG_UNIT_NODE_MAX);

    stLogCore *corehandler = &g_log_core_handler;
    LIST_INIT(&(corehandler->list[0]));
    LIST_INIT(&(corehandler->list[1]));
    corehandler->read_list = &(corehandler->list[1]);
    corehandler->write_list = &(corehandler->list[0]);
    SPIN_LOCK_INIT(&(corehandler->list_lock));

    corehandler->lastTime = 0;
    corehandler->existfiles = 0;
    LIST_INIT(&(corehandler->file_list));

    corehandler->isAlive = 1;
    THREAD_CREATE(&(corehandler->thread), NULL, log_core_process, NULL);
}

void log_core_destroy()
{
    stLogCore *corehandler = &g_log_core_handler;
    corehandler->isAlive = 0;
    THREAD_JOIN(corehandler->thread, NULL);

    SPIN_LOCK_DESTROY(&(g_log_core_handler.list_lock));

    log_mem_destory();
}


