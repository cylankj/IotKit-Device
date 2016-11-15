#ifdef _WIN32
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include "logger.h"
#include "common.h"

void *logger_lock_create()
{
    return NULL;
}

void logger_lock_acquire(void *lock)
{
}

void logger_lock_release(void *lock)
{
}

static void logger_writer(struct LOGGER_APPENDER *appender, int level, const char *buf, int len)
{
    if (buf != NULL)
    {
        printf(buf);
    }
}

void platform_init(void)
{
    WSADATA wsd;
    WSAStartup(MAKEWORD(1, 0), &wsd);

    my_logger_init(logger_writer);
    application_main(NULL);
}

#endif // _WIN32
