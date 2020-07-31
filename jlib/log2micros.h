#pragma once

#ifndef JLIB_LOG2_MICROS
#define JLIB_LOG2_MICROS

#ifndef JLIB_LOG2_ENABLED
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

#endif

#endif
