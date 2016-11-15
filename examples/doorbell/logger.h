#ifndef __IOTK_LOGGER_H__
#define __IOTK_LOGGER_H__

#include "logger/logger.h"

#define LOGGER_EXCLUDE_FILELINE 0x00000010

void my_logger_init(LOGGER_WRITER writer);

#endif // __IOTK_LOGGER_H__
