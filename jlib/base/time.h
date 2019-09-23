#pragma once

// for cross-platform

#include "config.h"
#include <time.h>
#include <stdint.h>


#ifdef JLIB_WINDOWS
#	include <windows.h>
	static inline struct tm* gmtime_r(const time_t* timep, struct tm* result)
	{
		gmtime_s(result, timep);
		return result;
	}
#else
#   include <sys/time.h> // gettimeofday
#endif


namespace jlib {


static constexpr int MICRO_SECONDS_PER_SECOND = 1000 * 1000;

/* FILETIME of Jan 1 1970 00:00:00. */
static constexpr uint64_t EPOCH = 116444736000000000UL;


static inline int64_t gettimeofdayUsec()
{
#ifdef JLIB_WINDOWS
	FILETIME ft = { 0 };

#if _WIN32_WINNT > _WIN32_WINNT_WIN7
	GetSystemTimePreciseAsFileTime(&ft); // win8 or later
#else // before win8
	SYSTEMTIME st;
	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);
#endif // _WIN32_WINNT > _WIN32_WINNT_WIN7

	ULARGE_INTEGER ularge;
	ularge.LowPart = ft.dwLowDateTime;
	ularge.HighPart = ft.dwHighDateTime;

	return (ularge.QuadPart - EPOCH) / 10L;

#elif defined(JLIB_LINUX)
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	int64_t seconds = tv.tv_sec;
	return seconds * MICRO_SECONDS_PER_SECOND + tv.tv_usec;
#endif // JLIB_WINDOWS

	return 0;
}

}
