#include <stdio.h>
#include <unistd.h>
#include "log.h"

int main(int argc, char *argv[])
{
    log_init();

    int i = 0;
    while(1)
    {
        LOG_INFO("hello, %d\n", i++);
        if (i >= 20000)
            break;
        sleep(1);
    }

    printf("log over\n");

    log_destory();
}
