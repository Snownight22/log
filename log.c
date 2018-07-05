#include <stdio.h>
//#include "log.h"
#include "log_config.h"

void log_init()
{
    log_config_init(NULL);
    log_core_init();
}

void log_destory()
{
    log_core_destroy();
    log_config_destory();
}
