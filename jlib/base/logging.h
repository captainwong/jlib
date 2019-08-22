#pragma once

#include "config.h"
#include "logstream.h"
#include "timestamp.h"
#include "currentthread.h"
#include <stdlib.h> // getenv

namespace jlib
{

class TimeZone;

class Logger
{
public:
	enum LogLevel {
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
		SourceFile(const char(&arr)[N]) : data_(arr), size_(N - 1) {
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
	Logger(SourceFile file, int line, LogLevel level, const char* func);
	Logger(SourceFile file, int line, bool toAbort);
	~Logger();

	LogStream& stream() { return impl_.stream_; }

	static LogLevel logLevel() { return logLevel_; }
	static void setLogLevel(LogLevel level);

	typedef void (*OutputFunc)(const char* msg, int len);
	typedef void (*FlushFunc)();
	static void setOutput(OutputFunc);
	static void setFlush(FlushFunc);
	static void setTimeZone(const TimeZone& tz);

private:

	class Impl
	{
	public:
		typedef Logger::LogLevel LogLevel;

		Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
		void formatTime();
		void finish();

		Timestamp time_;
		LogStream stream_;
		LogLevel level_;
		int line_;
		SourceFile basename_;
	};

	static LogLevel logLevel_;

	Impl impl_;
};


static void defaultOutput(const char* msg, int len) { fwrite(msg, 1, len, stdout); }
static void defaultFlush() { fflush(stdout); }

static Logger::OutputFunc g_output = defaultOutput;
static Logger::FlushFunc g_flush = defaultFlush;


namespace detail
{

static const char* LogLevelName[Logger::LogLevel::LOGLEVEL_COUNT] =
{
	"TRACE ",
	"DEBUG ",
	"INFO  ",
	"WARN  ",
	"ERROR ",
	"FATAL ",
};

static Logger::LogLevel initLogLevel() {
	if (::getenv("JLIB_LOGLEVEL_TRACE")) {
		return Logger::LogLevel::LOGLEVEL_TRACE;
	} else if (::getenv("JLIB_LOGLEVEL_DEBUG")) {
		return Logger::LogLevel::LOGLEVEL_DEBUG;
	} else {
		return Logger::LogLevel::LOGLEVEL_INFO;
	}
}

struct T
{
	explicit T(const char* str, unsigned int len)
		: str_(str)
		, len_(len)
	{
		assert(strlen(str) == len);
	}

	const char* str_;
	const unsigned int len_;
};

thread_local char t_errnobuf[512] = { 0 };
thread_local char t_time[64] = { 0 };
thread_local time_t t_lastSecond = 0;

} // detail


Logger::LogLevel Logger::logLevel_ = detail::initLogLevel();

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& f) {
	s.append(f.data_, f.size_); return s;
}

inline LogStream& operator<<(LogStream& s, detail::T t) {
	s.append(t.str_, t.len_); return s;
}

static const char* strerror_t(int savedErrno) {
	strerror_s(detail::t_errnobuf, savedErrno);
	return detail::t_errnobuf;
}


Logger::Impl::Impl(LogLevel level, int old_errno, const SourceFile& file, int line)
	: time_(Timestamp::now())
	, stream_()
	, level_(level)
	, line_(line)
	, basename_(file)
{
	formatTime();
	CurrentThread::tid();
	stream_ << detail::T(CurrentThread::tidString(), CurrentThread::tidStringLength());
	stream_ << detail::T(detail::LogLevelName[level], 6);
	if (old_errno != 0) {
		stream_ << strerror_t(old_errno) << " (errorno=" << old_errno << ") ";
	}
}

void Logger::Impl::formatTime()
{
	int64_t microSecsSinceEpoch = time_.microSecondsSinceEpoch();
	time_t seconds = static_cast<time_t>(microSecsSinceEpoch / Timestamp::MICRO_SECONDS_PER_SECOND);
	int microsecs = static_cast<int>(microSecsSinceEpoch % Timestamp::MICRO_SECONDS_PER_SECOND);
	if (seconds != detail::t_lastSecond) {
		detail::t_lastSecond = seconds;

	}
}


} // namespace jlib
