#ifndef LOGGER_MACROS_H
#define LOGGER_MACROS_H

#define LOGGER_LEVEL_VERBOSE        1
#define LOGGER_LEVEL_DEBUG          2
#define LOGGER_LEVEL_INFO           3
#define LOGGER_LEVEL_WARN           4
#define LOGGER_LEVEL_ERROR          5
#define LOGGER_LEVEL_FATAL          6
#define LOGGER_LEVEL_MASK           (0x00000007)
#define LOGGER_LEVEL_ALL            (0xFFFFFFFF)
#define LOGGER_ENDL                 "\r\n"

#if defined(_MSC_VER)
#define LOG(level, fmt, ...)        logger_printf(level, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGL(level, fmt, ...)       logger_printf(level, __FILE__, __LINE__, fmt LOGGER_ENDL, __VA_ARGS__)
#define LOGC(level, fmt, ...)       logger_catf(level, fmt, __VA_ARGS__)
#else
#define LOG(level, fmt, args...)    logger_printf(level, __FILE__, __LINE__, fmt, ##args)
#define LOGL(level, fmt, args...)   logger_printf(level, __FILE__, __LINE__, fmt LOGGER_ENDL, ##args)
#define LOGC(level, fmt, args...)   logger_catf(level, fmt, ##args)
#endif

#define LOGB(level, dat, len)       logger_printb(level, __FILE__, __LINE__, dat, len);
#define LOGBV(dat, len)             LOGB(LOGGER_LEVEL_VERBOSE, dat, len)
#define LOGBD(dat, len)             LOGB(LOGGER_LEVEL_DEBUG,   dat, len)
#define LOGBI(dat, len)             LOGB(LOGGER_LEVEL_INFO,    dat, len)
#define LOGBW(dat, len)             LOGB(LOGGER_LEVEL_WARN,    dat, len)
#define LOGBE(dat, len)             LOGB(LOGGER_LEVEL_ERROR,   dat, len)
#define LOGBF(dat, len)             LOGB(LOGGER_LEVEL_FATAL,   dat, len)

#if defined(_MSC_VER)

#define LOGF(fmt, ...)              LOG(LOGGER_LEVEL_FATAL,    fmt, __VA_ARGS__)
#define LOGE(fmt, ...)              LOG(LOGGER_LEVEL_ERROR,    fmt, __VA_ARGS__)
#define LOGW(fmt, ...)              LOG(LOGGER_LEVEL_WARN,     fmt, __VA_ARGS__)
#define LOGI(fmt, ...)              LOG(LOGGER_LEVEL_INFO,     fmt, __VA_ARGS__)
#define LOGD(fmt, ...)              LOG(LOGGER_LEVEL_DEBUG,    fmt, __VA_ARGS__)
#define LOGV(fmt, ...)              LOG(LOGGER_LEVEL_VERBOSE,  fmt, __VA_ARGS__)
#define LOGLF(fmt, ...)             LOGL(LOGGER_LEVEL_FATAL,   fmt, __VA_ARGS__)
#define LOGLE(fmt, ...)             LOGL(LOGGER_LEVEL_ERROR,   fmt, __VA_ARGS__)
#define LOGLW(fmt, ...)             LOGL(LOGGER_LEVEL_WARN,    fmt, __VA_ARGS__)
#define LOGLI(fmt, ...)             LOGL(LOGGER_LEVEL_INFO,    fmt, __VA_ARGS__)
#define LOGLD(fmt, ...)             LOGL(LOGGER_LEVEL_DEBUG,   fmt, __VA_ARGS__)
#define LOGLV(fmt, ...)             LOGL(LOGGER_LEVEL_VERBOSE, fmt, __VA_ARGS__)
#define LOGCF(fmt, ...)             LOGC(LOGGER_LEVEL_FATAL,   fmt, __VA_ARGS__)
#define LOGCE(fmt, ...)             LOGC(LOGGER_LEVEL_ERROR,   fmt, __VA_ARGS__)
#define LOGCW(fmt, ...)             LOGC(LOGGER_LEVEL_WARN,    fmt, __VA_ARGS__)
#define LOGCI(fmt, ...)             LOGC(LOGGER_LEVEL_INFO,    fmt, __VA_ARGS__)
#define LOGCD(fmt, ...)             LOGC(LOGGER_LEVEL_DEBUG,   fmt, __VA_ARGS__)
#define LOGCV(fmt, ...)             LOGC(LOGGER_LEVEL_VERBOSE, fmt, __VA_ARGS__)

#elif defined(__UCOS__)  

// workaround macros for hcac compiler
#define _LOGF(fmt, args...)         LOG(LOGGER_LEVEL_FATAL,    fmt, ##args)
#define _LOGE(fmt, args...)         LOG(LOGGER_LEVEL_ERROR,    fmt, ##args)
#define _LOGW(fmt, args...)         LOG(LOGGER_LEVEL_WARN,     fmt, ##args)
#define _LOGI(fmt, args...)         LOG(LOGGER_LEVEL_INFO,     fmt, ##args)
#define _LOGD(fmt, args...)         LOG(LOGGER_LEVEL_DEBUG,    fmt, ##args)
#define _LOGV(fmt, args...)         LOG(LOGGER_LEVEL_VERBOSE,  fmt, ##args)
#define _LOGLF(fmt, args...)        LOGL(LOGGER_LEVEL_FATAL,   fmt, ##args)
#define _LOGLE(fmt, args...)        LOGL(LOGGER_LEVEL_ERROR,   fmt, ##args)
#define _LOGLW(fmt, args...)        LOGL(LOGGER_LEVEL_WARN,    fmt, ##args)
#define _LOGLI(fmt, args...)        LOGL(LOGGER_LEVEL_INFO,    fmt, ##args)
#define _LOGLD(fmt, args...)        LOGL(LOGGER_LEVEL_DEBUG,   fmt, ##args)
#define _LOGLV(fmt, args...)        LOGL(LOGGER_LEVEL_VERBOSE, fmt, ##args)
#define _LOGCF(fmt, args...)        LOGC(LOGGER_LEVEL_FATAL,   fmt, ##args)
#define _LOGCE(fmt, args...)        LOGC(LOGGER_LEVEL_ERROR,   fmt, ##args)
#define _LOGCW(fmt, args...)        LOGC(LOGGER_LEVEL_WARN,    fmt, ##args)
#define _LOGCI(fmt, args...)        LOGC(LOGGER_LEVEL_INFO,    fmt, ##args)
#define _LOGCD(fmt, args...)        LOGC(LOGGER_LEVEL_DEBUG,   fmt, ##args)
#define _LOGCV(fmt, args...)        LOGC(LOGGER_LEVEL_VERBOSE, fmt, ##args)

#define LOGF(args...)               _LOGF(##args, "")
#define LOGE(args...)               _LOGE(##args, "")
#define LOGW(args...)               _LOGW(##args, "")
#define LOGI(args...)               _LOGI(##args, "")
#define LOGD(args...)               _LOGD(##args, "")
#define LOGV(args...)               _LOGV(##args, "")
#define LOGLF(args...)              _LOGLF(##args, "")
#define LOGLE(args...)              _LOGLE(##args, "")
#define LOGLW(args...)              _LOGLW(##args, "")
#define LOGLI(args...)              _LOGLI(##args, "")
#define LOGLD(args...)              _LOGLD(##args, "")
#define LOGLV(args...)              _LOGLV(##args, "")
#define LOGCF(args...)              _LOGCF(##args, "")
#define LOGCE(args...)              _LOGCE(##args, "")
#define LOGCW(args...)              _LOGCW(##args, "")
#define LOGCI(args...)              _LOGCI(##args, "")
#define LOGCD(args...)              _LOGCD(##args, "")
#define LOGCV(args...)              _LOGCV(##args, "")

#else // _MSC_VER

#define LOGF(fmt, args...)          LOG(LOGGER_LEVEL_FATAL,    fmt, ##args)
#define LOGE(fmt, args...)          LOG(LOGGER_LEVEL_ERROR,    fmt, ##args)
#define LOGW(fmt, args...)          LOG(LOGGER_LEVEL_WARN,     fmt, ##args)
#define LOGI(fmt, args...)          LOG(LOGGER_LEVEL_INFO,     fmt, ##args)
#define LOGD(fmt, args...)          LOG(LOGGER_LEVEL_DEBUG,    fmt, ##args)
#define LOGV(fmt, args...)          LOG(LOGGER_LEVEL_VERBOSE,  fmt, ##args)
#define LOGLF(fmt, args...)         LOGL(LOGGER_LEVEL_FATAL,   fmt, ##args)
#define LOGLE(fmt, args...)         LOGL(LOGGER_LEVEL_ERROR,   fmt, ##args)
#define LOGLW(fmt, args...)         LOGL(LOGGER_LEVEL_WARN,    fmt, ##args)
#define LOGLI(fmt, args...)         LOGL(LOGGER_LEVEL_INFO,    fmt, ##args)
#define LOGLD(fmt, args...)         LOGL(LOGGER_LEVEL_DEBUG,   fmt, ##args)
#define LOGLV(fmt, args...)         LOGL(LOGGER_LEVEL_VERBOSE, fmt, ##args)
#define LOGCF(fmt, args...)         LOGC(LOGGER_LEVEL_FATAL,   fmt, ##args)
#define LOGCE(fmt, args...)         LOGC(LOGGER_LEVEL_ERROR,   fmt, ##args)
#define LOGCW(fmt, args...)         LOGC(LOGGER_LEVEL_WARN,    fmt, ##args)
#define LOGCI(fmt, args...)         LOGC(LOGGER_LEVEL_INFO,    fmt, ##args)
#define LOGCD(fmt, args...)         LOGC(LOGGER_LEVEL_DEBUG,   fmt, ##args)
#define LOGCV(fmt, args...)         LOGC(LOGGER_LEVEL_VERBOSE, fmt, ##args)

#endif // _MSC_VER

#endif // LOGGER_MACROS_H
