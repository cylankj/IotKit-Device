#ifndef MSGPACK_PORT_H
#define MSGPACK_PORT_H

#if defined(_WIN32)
#include <assert.h>
#include <windows.h>
#endif

#if (CONFIG_MSGPACK_WITH_LOGGER == 1)
#include "logger/logger.h"
#else
#if defined(_WIN32)
#define LOGD(...)           printf(__VA_ARGS__)
#define LOGCD(...)          printf(__VA_ARGS__)
#else
#define LOGD(args...)
#define LOGCD(args...)
#endif
#endif

#ifndef ASSERT
#define ASSERT(x)           assert(x)
#endif

#ifndef PRId64
#define PRId64              "lld"
#endif

#ifndef PRIu64
#define PRIu64              "llu"
#endif

#ifndef LLONG_MIN
#define LLONG_MIN           -9223372036854775807LL
#endif

#ifndef LLONG_MAX
#define LLONG_MAX           9223372036854775807LL
#endif

#ifdef cc3200
#define MSGPACK_MALLOC      mem_Malloc
#define MSGPACK_FREE        mem_Free
#else
#define MSGPACK_MALLOC      malloc
#define MSGPACK_FREE        free
#endif

#endif // MSGPACK_PORT_H
