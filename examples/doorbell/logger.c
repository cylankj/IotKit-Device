#include <time.h>
#include <stdio.h>
#include <ctype.h>

#include "logger.h"

#ifdef  _WIN32
#define snprintf    _snprintf
#endif

#define DUMP_HEX_HEX_BUF_SIZE       (16 * 3 + 5)
#define DUMP_HEX_BIN_BUF_SIZE       (16 + 5)

static char *s_log_levels[] =
{
    "VERB ",
    "DEBUG",
    "INFO ",
    "WARN ",
    "ERROR",
    "FATAL"
};

static struct LOGGER_LAYOUT s_layout;
static struct LOGGER_APPENDER s_appender;

static int logger_format_bin(struct LOGGER_LAYOUT *layout, char *buf, int size, int level, const char *file, unsigned int line, const void *dat, int len)
{
    int n;
    int lv;
    int offset = 0;
    struct tm t;
    time_t sec;
    const char *filename = NULL;
    const unsigned char *p = (const unsigned char *)dat;

	time(&sec);
    t = *localtime(&sec);

    n = snprintf(buf + offset, size - offset, "%02d/%02d %02d:%02d:%02d ",
        t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    //n = snprintf(buf + offset, size - offset, "%02d:%02d:%02d ",
    //    t.tm_hour, t.tm_min, t.tm_sec);


    if (n > 0)
    {
        offset += n;
    }

    // level
    lv = level & LOGGER_LEVEL_MASK;
    lv = lv > LOGGER_LEVEL_FATAL ? LOGGER_LEVEL_FATAL : lv;
    lv = lv < LOGGER_LEVEL_VERBOSE ? LOGGER_LEVEL_VERBOSE : lv;
    lv -= 1;
    n = snprintf(buf + offset, size - offset, "%s ", s_log_levels[lv]);

    if (n > 0)
    {
        offset += n;
    }

    // position
    if (file != NULL && line > 0 && LOGGER_EXCLUDE_FILELINE != (level & LOGGER_EXCLUDE_FILELINE))
    {
        filename = strrchr(file, LOGGER_PATH_SLASH);

        if (filename == NULL)
        {
            filename = file;
        }
        else
        {
            filename = filename + 1;
        }

        n = snprintf(buf + offset, size - offset, "[%-8.8s:%5d] ", filename, line);

        if (n > 0)
        {
            offset += n;
        }
    }

    // put address
    offset += snprintf(buf + offset, size - offset, "[%08X] ", (unsigned int)dat);

    // put hex
    for (n = 0; n < len; ++n)
    {
        offset += snprintf(buf + offset, size - offset, "%02X ", p[n]);
    }

    // paddings
    for (; n < 16; ++n)
    {
        offset += snprintf(buf + offset, size - offset, "   ");
    }

    // put char
    for (n = 0; n < len; ++n)
    {
        offset += snprintf(buf + offset, size - offset, "%c", isprint(p[n]) ? p[n] : '.');
    }

    offset += snprintf(buf + offset, size - offset, LOGGER_ENDL);
    return offset;
}

static int logger_format_str(struct LOGGER_LAYOUT *layout, char *buf, int size, int level, const char *file, unsigned int line, const char *fmt, va_list ap)
{
    int n;
    int lv;
    int offset = 0;
    struct tm t;
	time_t sec;
	const char *filename = NULL;

	time(&sec);
	t = *localtime(&sec);

    n = snprintf(buf + offset, size - offset, "%02d/%02d %02d:%02d:%02d ",
        t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    //n = snprintf(buf + offset, size - offset, "%02d:%02d:%02d ",
    //    t.tm_hour, t.tm_min, t.tm_sec);


    if (n > 0)
    {
        offset += n;
    }

    // level
    lv = level & LOGGER_LEVEL_MASK;
    lv = lv > LOGGER_LEVEL_FATAL ? LOGGER_LEVEL_FATAL : lv;
    lv = lv < LOGGER_LEVEL_VERBOSE ? LOGGER_LEVEL_VERBOSE : lv;
    lv -= 1;
    n = snprintf(buf + offset, size - offset, "%s ", s_log_levels[lv]);

    if (n > 0)
    {
        offset += n;
    }

    // position
    if (file != NULL && line > 0 && LOGGER_EXCLUDE_FILELINE != (level & LOGGER_EXCLUDE_FILELINE))
    {
        filename = strrchr(file, LOGGER_PATH_SLASH);

        if (filename == NULL)
        {
            filename = file;
        }
        else
        {
            filename = filename + 1;
        }

        n = snprintf(buf + offset, size - offset, "[%-8.8s:%5d] ", filename, line);

        if (n > 0)
        {
            offset += n;
        }
    }

    return vsnprintf(buf + offset, size - offset, fmt, ap);
}

void my_logger_init(LOGGER_WRITER writer)
{
    logger_init();

    // layout
    memset(&s_layout, 0, sizeof(s_layout));
    s_layout.format_str = logger_format_str;
    s_layout.format_bin = logger_format_bin;

    // appender
    memset(&s_appender, 0, sizeof(s_appender));
    s_appender.writer = writer;
    s_appender.level_mask = LOGGER_LEVEL_ALL;

    logger_layout_add_appender(&s_layout, &s_appender);
    logger_add_layout(&s_layout);
}
