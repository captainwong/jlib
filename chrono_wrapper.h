#pragma once

#include <chrono>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace jlib {

#ifdef __GNUG__
inline void localtime_s(struct tm* tmtm, const time_t* t) {
	memcpy(tmtm, localtime(t), sizeof(tm));
}
#else

#endif

// section: 
// 0 for YYYY-mm-dd HH:MM:SS
// 1 for YYYY-mm-dd 
// 2 for HH:MM:SS
inline std::wstring time_t_to_wstring(time_t t, int section = 0)
{
	wchar_t wtime[32] = { 0 };
	struct tm tmtm;
	localtime_s(&tmtm, &t);
	if (t == -1) {
		t = time(nullptr);
		localtime_s(&tmtm, &t);
	}

	if (section == 0) {
		wcsftime(wtime, 32, L"%Y-%m-%d %H:%M:%S", &tmtm);
	} else if (section == 1) {
		wcsftime(wtime, 32, L"%Y-%m-%d", &tmtm);
	} else {
		wcsftime(wtime, 32, L"%H:%M:%S", &tmtm);
	}

	return std::wstring(wtime);
}

// section: 
// 0 for YYYY-mm-dd HH:MM:SS
// 1 for YYYY-mm-dd 
// 2 for HH:MM:SS
inline std::string time_t_to_string(time_t t, int section = 0)
{
	char stime[32] = { 0 };
	struct tm tmtm;
	localtime_s(&tmtm, &t);
	if (t == -1) {
		t = time(nullptr);
		localtime_s(&tmtm, &t);
	}

	if (section == 0) {
		strftime(stime, 32, "%Y-%m-%d %H:%M:%S", &tmtm);
	} else if (section == 1) {
		strftime(stime, 32, "%Y-%m-%d", &tmtm);
	} else {
		strftime(stime, 32, "%H:%M:%S", &tmtm);
	}

	return std::string(stime);
}

inline std::string time_point_to_string(const std::chrono::system_clock::time_point& tp, bool with_milliseconds = false)
{
	std::stringstream ss;
	auto t = std::chrono::system_clock::to_time_t(tp);
	std::tm* tm = std::localtime(&t);
	ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
	if (with_milliseconds) {
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
		auto millis = ms.count() % 1000;
		ss << '.' << std::setw(3) << std::setfill('0') << millis;
	}
	return ss.str();
}

inline std::wstring time_point_to_wstring(const std::chrono::system_clock::time_point& tp, bool with_milliseconds = false)
{
	std::wstringstream ss;
	auto t = std::chrono::system_clock::to_time_t(tp);
	std::tm* tm = std::localtime(&t);
	ss << std::put_time(tm, L"%Y-%m-%d %H:%M:%S");
	if (with_milliseconds) {
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
		auto millis = ms.count() % 1000;
		ss << L'.' << std::setw(3) << std::setfill(L'0') << millis;
	}
	return ss.str();
}

inline std::chrono::system_clock::time_point string_to_time_point(const std::string& s)
{
	std::tm tm = { 0 };
	std::istringstream ss(s);
	ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
	return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

inline std::chrono::system_clock::time_point wstring_to_time_point(const std::wstring& s)
{
	std::tm tm = { 0 };
	std::wistringstream ss(s);
	ss >> std::get_time(&tm, L"%Y-%m-%d %H:%M:%S");
	return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

inline std::string now_to_string(bool with_milliseconds = false)
{
	return time_point_to_string(std::chrono::system_clock::now(), with_milliseconds);
}

inline std::wstring now_to_wstring(bool with_milliseconds = false)
{
	return time_point_to_wstring(std::chrono::system_clock::now(), with_milliseconds);
}



};
