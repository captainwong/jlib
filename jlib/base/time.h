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


/********** Windows Only *******/

#ifdef JLIB_WINDOWS

static inline tm systemTimeToTm(const SYSTEMTIME& st)
{
	tm tm;
	tm.tm_year	= st.wYear - 1900;
	tm.tm_mon	= st.wMonth - 1;
	tm.tm_mday	= st.wDay;
	tm.tm_hour	= st.wHour;
	tm.tm_min	= st.wMinute;
	tm.tm_sec	= st.wSecond;
	tm.tm_isdst = -1;
	return tm;
}

static inline SYSTEMTIME tmToSystemTime(const tm& tm)
{
	SYSTEMTIME st = { 0 };
	st.wYear	= tm.tm_year + 1900;
	st.wMonth	= tm.tm_mon + 1;
	st.wDay		= tm.tm_mday;
	st.wHour	= tm.tm_hour;
	st.wMinute	= tm.tm_min;
	st.wSecond	= tm.tm_sec;
	st.wDayOfWeek = tm.tm_wday;
	return st;
}

static inline time_t systemTimeToTime_t(const SYSTEMTIME& st)
{
	tm tm = systemTimeToTm(st);
	return mktime(&tm);
}

static inline SYSTEMTIME time_tToSystemTime(time_t t)
{
	tm tm; gmtime_s(&tm, &t);
	return tmToSystemTime(tm);
}

static inline SYSTEMTIME time_tToSystemTimeLocal(time_t t)
{
	tm tm; localtime_s(&tm, &t);
	return tmToSystemTime(tm);
}

#endif // JLIB_WINDOWS

}
