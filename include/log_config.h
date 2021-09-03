#ifndef _LOG_CONFIG_H_
#define _LOG_CONFIG_H_

#define CONFIG_ITEM_MAX    (100)
#define CONFIG_LENGTH_MAX    (100)
#define CONFIG_FILE_LENGTH_MAX    (256)
#define CONFIG_LINE_MAX    (1024)

#define CONFIG_TYPE_BOOL    (1)
#define CONFIG_TYPE_INT    (2)
#define CONFIG_TYPE_STRING    (3)

typedef struct logItem
{
    char key[CONFIG_LINE_MAX];
    char value[CONFIG_LINE_MAX];
    int type;
}stLogItem;

typedef struct logConfig
{
    char fileName[CONFIG_FILE_LENGTH_MAX];
    unsigned char log_level;
    unsigned char log_console;
    unsigned char log_file;
    unsigned char rolling_filenums;
    unsigned char rolling_day;
    unsigned char rolling_hour;
    unsigned char rolling_min;
    unsigned char rolling_weekly;
    unsigned char rolling_month;
    int logUnitCount;
    char log_filename[CONFIG_FILE_LENGTH_MAX];
}stLogConfig;


void* log_config_handler_get();
int log_config_unitCount_get();

void log_config_init(char* config_file);
void log_config_destory();

#endif
