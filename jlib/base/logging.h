#pragma once

#include "config.h"
#include "logstream.h"
#include "timestamp.h"
#include "timezone.h"
#include "currentthread.h"
#include <stdlib.h> // getenv
#include <string.h> // strerror_s

namespace jlib
{

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
#ifdef JLIB_WINDOWS
			const char* slash = strrchr(data_, '\\');
#else
			const char* slash = strrchr(data_, '/');
#endif
			if (slash) { data_ = slash + 1; }
			size_ -= static_cast<int>(data_ - arr);
		}

		explicit SourceFile(const char* filename) : data_(filename) {
#ifdef JLIB_WINDOWS
			const char* slash = strrchr(data_, '\\');
#else
			const char* slash = strrchr(data_, '/');
#endif
			if (slash) { data_ = slash + 1; }
			size_ = static_cast<int>(strlen(data_));
		}

		const char* data_;
		int size_;
	};

	Logger(SourceFile file, int line) : impl_(LogLevel::LOGLEVEL_INFO, 0, file, line) {}
	Logger(SourceFile file, int line, LogLevel level) : impl_(level, 0, file, line) {}
	Logger(SourceFile file, int line, LogLevel level, const char* func) : impl_(level, 0, file, line) { impl_.stream_ << func << ' '; }
	Logger(SourceFile file, int line, bool toAbort) : impl_(toAbort ? LogLevel::LOGLEVEL_FATAL : LOGLEVEL_ERROR, errno, file, line) {}

	~Logger() {
		impl_.finish();
		const auto& buf = impl_.stream_.buffer();
		outputFunc_(buf.data(), buf.length());
		if (impl_.level_ == LOGLEVEL_FATAL) {
			flushFunc_();
			abort();
		}
	}

	LogStream& stream() { return impl_.stream_; }

	static LogLevel logLevel() { return logLevel_; }
	static void setLogLevel(LogLevel level) { logLevel_ = level; }

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

	static OutputFunc outputFunc_;
	static FlushFunc flushFunc_;
	static LogLevel logLevel_;
	static const TimeZone* timeZone_;

	Impl impl_;
};


/******** log micros *********/

#define LOG_TRACE if (jlib::Logger::logLevel() <= jlib::Logger::LogLevel::LOGLEVEL_TRACE) \
	jlib::Logger(__FILE__, __LINE__, jlib::Logger::LogLevel::LOGLEVEL_TRACE, __func__).stream()

#define LOG_DEBUG if (jlib::Logger::logLevel() <= jlib::Logger::LogLevel::LOGLEVEL_DEBUG) \
	jlib::Logger(__FILE__, __LINE__, jlib::Logger::LogLevel::LOGLEVEL_DEBUG, __func__).stream()

#define LOG_INFO if (jlib::Logger::logLevel() <= jlib::Logger::LogLevel::LOGLEVEL_INFO) \
	jlib::Logger(__FILE__, __LINE__, jlib::Logger::LogLevel::LOGLEVEL_INFO).stream()


#define LOG_WARN jlib::Logger(__FILE__, __LINE__, jlib::Logger::LogLevel::LOGLEVEL_WARN).stream()
#define LOG_ERROR jlib::Logger(__FILE__, __LINE__, jlib::Logger::LogLevel::LOGLEVEL_ERROR).stream()
#define LOG_FATAL jlib::Logger(__FILE__, __LINE__, jlib::Logger::LogLevel::LOGLEVEL_FATAL).stream()
#define LOG_SYSERR jlib::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL jlib::Logger(__FILE__, __LINE__, true).stream()


namespace detail
{

static void defaultOutput(const char* msg, int len) { fwrite(msg, 1, len, stdout); }
static void defaultFlush() { fflush(stdout); }

static const char* LogLevelName[Logger::LogLevel::LOGLEVEL_COUNT] = {
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
static constexpr unsigned int T_TIME_STR_LEN = 17;

} // detail


/******** static initializer *********/

Logger::OutputFunc Logger::outputFunc_ = detail::defaultOutput;
Logger::FlushFunc Logger::flushFunc_ = detail::defaultFlush;
Logger::LogLevel Logger::logLevel_ = detail::initLogLevel();
const jlib::TimeZone* Logger::timeZone_ = date::locate_zone("Etc/UTC");


/******** LogStream operators *********/

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


/******** Logger::Impl *********/

Logger::Impl::Impl(LogLevel level, int old_errno, const SourceFile& file, int line)
	: time_(nowTimestamp())
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
	//int64_t microSecsSinceEpoch = time_.microSecondsSinceEpoch();
	//time_t seconds = static_cast<time_t>(microSecsSinceEpoch / MICRO_SECONDS_PER_SECOND);
	//int microsecs = static_cast<int>(microSecsSinceEpoch % MICRO_SECONDS_PER_SECOND);
	//if (seconds != detail::t_lastSecond) {
	//	detail::t_lastSecond = seconds;
	//	struct tm tm_time;
	//	if (timeZone_.valid()) {
	//		// TODO
	//	} else {
	//		gmtime_r(&seconds, &tm_time);
	//	}

	//	int len = snprintf(detail::t_time, sizeof(detail::t_time), "%4d%02d%02d %02d:%02d:%02d",
	//					   tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
	//					   tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
	//	assert(len == detail::T_TIME_STR_LEN); (void)len;
	//}

	//if (timeZone_.valid()) {
	//	// TODO
	//} else {
	//	Format us(".%06dZ ", microsecs); assert(us.length() == 9);
	//	stream_ << detail::T(detail::t_time, detail::T_TIME_STR_LEN) << detail::T(us.data(), 9);
	//}

	stream_ << format("%F %T(%Z) ", time_);
}

void Logger::Impl::finish()
{
	stream_ << " - " << basename_ << ':' << line_ << '\n';
}


} // namespace jlib
