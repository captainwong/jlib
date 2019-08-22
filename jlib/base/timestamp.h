#pragma once

#include "config.h"
#include "copyable.h"
#include <boost/operators.hpp>
#include <stdint.h> // int64_t
#include <string> // std::string
#include <inttypes.h> // PRId64
#include <stdio.h> // snprintf
#include <time.h> // gmtime_r

//#define _WIN32_WINNT _WIN32_WINNT_WIN7

#ifdef JLIB_WINDOWS
#	include <windows.h>
	static inline struct tm* gmtime_r(const time_t* timep, struct tm* result)
	{
		gmtime_s(result, timep);
		return result;
	}

    // https://blogs.msdn.microsoft.com/vcblog/2016/03/30/optimizing-the-layout-of-empty-base-classes-in-vs2015-update-2-3/
#   define ENABLE_EBO __declspec(empty_bases)

#elif defined(JLIB_LINUX)
#   include <sys/time.h> // gettimeofday
#   define ENABLE_EBO
#endif

namespace jlib
{

class ENABLE_EBO Timestamp : public jlib::copyable 
						   , public boost::equality_comparable<Timestamp>
						   , public boost::less_than_comparable<Timestamp>
{
public:
	Timestamp()
		: microSecondsSinceEpoch_(0)
	{}

    explicit Timestamp(int64_t microSecondsSinceEpoch)
        : microSecondsSinceEpoch_(microSecondsSinceEpoch)
    {}

	static const int MICRO_SECONDS_PER_SECOND = 1000 * 1000;

    void swap(Timestamp& rhs) {
        std::swap(microSecondsSinceEpoch_, rhs.microSecondsSinceEpoch_);
    }

    std::string toString() const {
		char buff[32] = {};
		int64_t seconds = microSecondsSinceEpoch_ / MICRO_SECONDS_PER_SECOND;
		int64_t microseconds = microSecondsSinceEpoch_ % MICRO_SECONDS_PER_SECOND;
		snprintf(buff, sizeof(buff) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
		return buff;
    }

	std::string toFormattedString(bool showMicroSeconds = true) {
		char buf[64] = { 0 };
		time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / MICRO_SECONDS_PER_SECOND);
		struct tm tm_time;
		gmtime_r(&seconds, &tm_time);

		if (showMicroSeconds) {
			int microseconds = static_cast<int>(microSecondsSinceEpoch_ % MICRO_SECONDS_PER_SECOND);
			snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
					 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
					 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
					 microseconds);
		} else {
			snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
					 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
					 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
		}
		return buf;
	}

	bool valid() const { return microSecondsSinceEpoch_ > 0; }

	int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
	time_t secondsSinceEpoch() const { return static_cast<time_t>(microSecondsSinceEpoch_ / MICRO_SECONDS_PER_SECOND); }


	static Timestamp now() {
#ifdef JLIB_WINDOWS
		/* FILETIME of Jan 1 1970 00:00:00. */
		static constexpr unsigned __int64 EPOCH = 116444736000000000UL;

		FILETIME ft;

#if		_WIN32_WINNT > _WIN32_WINNT_WIN7
		GetSystemTimePreciseAsFileTime(&ft); // win8 or later
#else
		SYSTEMTIME st;
		GetSystemTime(&st);
		SystemTimeToFileTime(&st, &ft);
#endif 

		ULARGE_INTEGER ularge;
		ularge.LowPart = ft.dwLowDateTime;
		ularge.HighPart = ft.dwHighDateTime;

		return Timestamp((ularge.QuadPart - EPOCH) / 10L);
#elif defined(JLIB_LINUX)
		struct timeval tv;
		gettimeofday(&tv, nullptr);
		int64_t seconds = tv.tv_sec;
		return Timestamp(seconds * MICRO_SECONDS_PER_SECOND + tv.tv_usec);
#endif
	}

	static Timestamp invalid() { return Timestamp(); }

	static Timestamp fromTime_t(time_t t) {
		return fromTime_t(t, 0);
	}

	static Timestamp fromTime_t(time_t t, int microSeconds) {
		return Timestamp(static_cast<int64_t>(t) * MICRO_SECONDS_PER_SECOND + microSeconds);
	}


private:
    int64_t microSecondsSinceEpoch_ = 0;
};

static_assert(sizeof(Timestamp) == sizeof(int64_t), "Expected Timestamp to be the same size as int64_t");

inline bool operator<(Timestamp lhs, Timestamp rhs) {
	return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs) {
	return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

/**
* @brief Get time diff of two Timestamps, return diff in seconds
* @param high, low
* @return (high - low) in seconds
*/
inline double timeDifference(Timestamp high, Timestamp low) {
	int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
	return static_cast<double>(diff) / Timestamp::MICRO_SECONDS_PER_SECOND;
}

inline Timestamp addSeconds(Timestamp t, double seconds) {
	int64_t delta = static_cast<int64_t>(seconds * Timestamp::MICRO_SECONDS_PER_SECOND);
	return Timestamp(t.microSecondsSinceEpoch() + delta);
}

} // namespace jlib
