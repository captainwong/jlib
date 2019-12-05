#pragma once 

#if defined(DISABLE_JLIB_LOG2) && !defined(JLIB_DISABLE_LOG)
#define JLIB_DISABLE_LOG
#endif

#ifndef JLIB_DISABLE_LOG
#include "log2.h"
#else // JLIB_DISABLE_LOG
#define init_logger
#define JLOG_DBUG
#define JLOG_INFO
#define JLOG_WARN
#define JLOG_ERRO
#define JLOG_CRTC
#define JLOG_ALL

class range_log {
public:
	range_log() {}
	range_log(const char*) {}
};

#define AUTO_LOG_FUNCTION

#define dump_hex
#define dump_asc
#define JLOG_HEX
#define JLOG_ASC
#endif // JLIB_DISABLE_LOG
