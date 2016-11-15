#ifndef LOGGER_CONFIG_H
#define LOGGER_CONFIG_H

#ifdef _MSC_VER
#define LOGGER_PATH_SLASH       '\\'
#else
#define LOGGER_PATH_SLASH       '/'
#endif

#if defined(cc3200)
#define LOGGER_THREAD_SAFE      1
#endif

#ifndef LOGGER_THREAD_SAFE
#define LOGGER_THREAD_SAFE      0
#endif

#ifndef LOGGER_BUFFER_SIZE
#define LOGGER_BUFFER_SIZE      256
#endif

#endif
