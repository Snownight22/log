#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "log.h"
#include "log_config.h"

stLogConfig *g_log_config_handler = NULL;

stLogItem g_log_item[] = 
{
    {"logLevel", "5", CONFIG_TYPE_INT},
    {"logConsole", "true", CONFIG_TYPE_BOOL},
    {"logFile", "true", CONFIG_TYPE_BOOL},
    {"logFileName", "error.log", CONFIG_TYPE_STRING},
    {"logRollingFileNums", "0", CONFIG_TYPE_INT},
    {"logRollingDay", "false", CONFIG_TYPE_BOOL},
    {"logRollingHour", "false", CONFIG_TYPE_BOOL},
    {"logRollingMin", "false", CONFIG_TYPE_BOOL},
    {"logRollingMonth", "false", CONFIG_TYPE_BOOL},
    {"logRollingWeekly", "false", CONFIG_TYPE_BOOL},
    {"logUnitCount", "2048", CONFIG_TYPE_INT},
};

static int log_config_int_check(char *value)
{
    int i;

    for (i = 0; i < strlen(value); i++)
    {
        if (!isdigit(value[i]))
            return LOG_ERR_FAIL;
    }

    return LOG_ERR_OK;
}

static int log_config_bool_check(char *value)
{
    char boolstr[6] = {0};
    int i;

    if (strlen(value) > 5)
        return LOG_ERR_FAIL;

    for (i = 0; i < strlen(value); i++)
        boolstr[i] = tolower(value[i]);

    if (strcmp(boolstr, "true") && strcmp(boolstr, "false"))
        return LOG_ERR_FAIL;

    return LOG_ERR_OK;
}

static int log_config_string_check(char *value)
{
    return LOG_ERR_OK;
}

int log_config_type_check(char *value, int type)
{
    switch(type)
    {
        case CONFIG_TYPE_BOOL:
            return log_config_bool_check(value);
        case CONFIG_TYPE_INT:
            return log_config_int_check(value);
        case CONFIG_TYPE_STRING:
            return log_config_string_check(value);
        default:
            return LOG_ERR_FAIL;
    }

    return LOG_ERR_OK;
}

int log_config_parse_line(char* line, char* key, char* value)
{
    char *token;
    int i;
    int j;

    memset(key, 0, CONFIG_LINE_MAX);
    memset(value, 0, CONFIG_LINE_MAX);

    token = strtok(line, "=");
    if (NULL == token)
        return LOG_ERR_FAIL;

    i = 0;
    while (isblank(token[i]))
        i++;
    j = 0;
    while ((!isblank(token[i])) && token[i] != 0 && token[i] != '\r' && token[i] != '\n')
        key[j++] = token[i++];

    token = strtok(NULL, "=");
    if (NULL == token)
        return LOG_ERR_FAIL;

    i = 0;
    while (isblank(token[i]))
        i++;
    j = 0;
    while ((!isblank(token[i])) && token[i] != 0 && token[i] != '\r' && token[i] != '\n')
        value[j++] = tolower(token[i++]);

    return LOG_ERR_OK;
}

void log_config_var_init()
{
    stLogConfig *config_handler = g_log_config_handler;
    if (NULL == config_handler)
        return ;

    config_handler->log_level = atoi(g_log_item[0].value);
    if (!strcmp(g_log_item[1].value, "true"))
        config_handler->log_console = 1;
    else
        config_handler->log_console = 0;

    if (!strcmp(g_log_item[2].value, "true"))
        config_handler->log_file = 1;
    else
        config_handler->log_file = 0;

    snprintf(config_handler->log_filename, CONFIG_FILE_LENGTH_MAX, g_log_item[3].value);
    config_handler->rolling_filenums = atoi(g_log_item[4].value);

    if (!strcmp(g_log_item[5].value, "true"))
        config_handler->rolling_day = 1;
    else
        config_handler->rolling_day = 0;

    if (!strcmp(g_log_item[6].value, "true"))
        config_handler->rolling_hour = 1;
    else
        config_handler->rolling_hour = 0;

    if (!strcmp(g_log_item[7].value, "true"))
        config_handler->rolling_min = 1;
    else
        config_handler->rolling_min = 0;

    if (!strcmp(g_log_item[8].value, "true"))
        config_handler->rolling_month = 1;
    else
        config_handler->rolling_month = 0;

    if (!strcmp(g_log_item[9].value, "true"))
        config_handler->rolling_weekly = 1;
    else
        config_handler->rolling_weekly = 0;

    config_handler->logUnitCount = atoi(g_log_item[10].value);
}

int log_config_read()
{
    FILE *fp = NULL;
    char config_line[CONFIG_LINE_MAX];
    char key[CONFIG_LINE_MAX];
    char value[CONFIG_LINE_MAX];
    int i;
    int item_len = sizeof(g_log_item) / sizeof(stLogItem);

    if (NULL == (fp = fopen(g_log_config_handler->fileName, "rb")))
    {
        fprintf(stderr, "log file read error\n");
        return LOG_ERR_FAIL;
    }

    while(fgets(config_line, 1024, fp))
    {
        log_config_parse_line(config_line, key, value);
        for (i = 0; i < item_len; i++)
        {
            if (!strcmp(g_log_item[i].key, key))
            {
                if (LOG_ERR_OK == log_config_type_check(value, g_log_item[i].type))
                    strcpy(g_log_item[i].value, value);
                break;
            }
        }

    }

    fclose(fp);

    log_config_var_init();

    return LOG_ERR_OK;
}

void log_config_init(char* config_file)
{
    if (NULL == g_log_config_handler)
    {
        g_log_config_handler = (stLogConfig *) malloc(sizeof(stLogConfig));
        if (NULL == g_log_config_handler)
        {
            fprintf(stderr, "log handler malloc error!\n");
            return ;
        }

        if (NULL == config_file)
            snprintf(g_log_config_handler->fileName, CONFIG_FILE_LENGTH_MAX - 1, "log.ini");
        else
            snprintf(g_log_config_handler->fileName, CONFIG_FILE_LENGTH_MAX - 1, config_file);

        log_config_read();
    }
}

void log_config_destory()
{
    if (NULL != g_log_config_handler)
    {
        free(g_log_config_handler);
    }

    g_log_config_handler = NULL;
}

int log_config_unitCount_get()
{
    if (NULL != g_log_config_handler)
        return g_log_config_handler->logUnitCount;

    return 0;
}

void* log_config_handler_get()
{
    return g_log_config_handler;
}

