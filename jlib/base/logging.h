#pragma once

#include "logstream.h"
#include "timestamp.h"

namespace jlib
{

class TimeZone;

class Logger
{
public:
    enum class LogLevel {
        LOGLEVEL_TRACE,
        LOGLEVEL_DEBUG,
        LOGLEVEL_INFO,
        LOGLEVEL_WARN,
        LOGLEVEL_ERROR,
        LOGLEVEL_FATAL,
        LOGLEVEL_COUNT,
    };

    // compile time calculation of basename of source file
    struct SourceFile
    {
        template <int N>
        SourceFile(const char (&arr)[N]) : data_(arr) , size_(N - 1) {
            const char* slash = strrchr(data_, '/');
            if (slash) { data_ = slash + 1; }
            size_ -= static_cast<int>(data_ - arr);
        }

        explicit SourceFile(const char* filename) : data_(filename) {
            const char* slash = strrchr(filename, '/');
            if (slash) { data_ = slash + 1; }
            size_ = static_cast<int>(strlen(data_));
        }

        const char* data_;
        int size_;
    };

    Logger(SourceFile file, int line);
	Logger(SourceFile file, int line, LogLevel level);
	Logger(SourceFile file, int line, LogLevel level, const char *func);
	Logger(SourceFile file, int line, bool toAbort);
	~Logger();

	LogStream &stream() { return impl_.stream_; }

	static LogLevel logLevel();
	static void setLogLevel(LogLevel level);

	typedef void (*OutputFunc)(const char *msg, int len);
	typedef void (*FlushFunc)();
	static void setOutput(OutputFunc);
	static void setFlush(FlushFunc);
	static void setTimeZone(const TimeZone &tz);

private:
    class Impl
    {
    public:

    };
}

} // namespace jlib
