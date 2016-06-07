#pragma once

#include <chrono>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace jlib {

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
