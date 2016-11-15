//
// logger library by X.J.
//
#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>
#include <string.h>

#include "logger/logger_config.h"
#include "logger/logger_marcos.h"

struct LOGGER_APPENDER;
struct LOGGER_LAYOUT;

typedef void (*LOGGER_WRITER)(struct LOGGER_APPENDER *appender, int level, const char *buf, int len);

struct LOGGER_APPENDER
{
    int level_mask;
    struct LOGGER_APPENDER *next;
    LOGGER_WRITER writer;
};

typedef int (*LOGGER_FORMATTER_STR)(struct LOGGER_LAYOUT *layout, char *buf, int n, int level, const char *file, unsigned int line, const char *fmt, va_list ap);
typedef int (*LOGGER_FORMATTER_BIN)(struct LOGGER_LAYOUT *layout, char *buf, int n, int level, const char *file, unsigned int line, const void *dat, int len);

struct LOGGER_LAYOUT
{
    struct LOGGER_LAYOUT *next;
    LOGGER_FORMATTER_STR format_str;
    LOGGER_FORMATTER_BIN format_bin;
    struct LOGGER_APPENDER *appenders;
};

void logger_printb(int level, const char *file, unsigned int line, const void *buf, int len);

void logger_printf(int level, const char *file, unsigned int line, const char *fmt, ...);

void logger_catf(int level, const char *fmt, ...);

void logger_layout_add_appender(struct LOGGER_LAYOUT *layout, struct LOGGER_APPENDER *appender);

void logger_add_layout(struct LOGGER_LAYOUT *layout);

void logger_set_level_mask(int level_mask);

void logger_init(void);

#endif // LOGGER_H
