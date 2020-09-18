#pragma once

#include <string>
#include <algorithm> 
#include <cctype>
#include <locale>

namespace jlib
{


/**************************** trim ***************************/
// Taken from https://stackoverflow.com/a/217605/2963736
// Thanks https://stackoverflow.com/users/13430/evan-teran

template <typename StringType>
inline void ltrim(StringType& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch); }));
}

template <typename StringType>
inline void rtrim(StringType& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch); }).base(), s.end());
}

template <typename StringType>
inline void trim(StringType& s) { ltrim(s); rtrim(s); }

template <typename StringType>
inline StringType ltrim_copy(StringType s) { ltrim(s); return s; }

template <typename StringType>
inline StringType rtrim_copy(StringType s) { rtrim(s); return s; }

template <typename StringType>
inline StringType trim_copy(StringType s) { trim(s); return s; }


/**************************** join ***************************/
/**
* @brief join 字符串
* @note StringType 可以为std::string或std::wstring
* @note StringContainer 必须为上述StringType的iterable容器类，如vector，list
*/
template <typename StringType, typename StringContainer>
inline StringType join(const StringContainer& container, const StringType& conjunction = StringType())
{
	StringType result;
	auto itBegin = container.cbegin();
	auto itEnd = container.cend();
	if (itBegin != itEnd) {
		result = *itBegin;
		itBegin++;
	}
	for (; itBegin != itEnd; itBegin++) {
		result += conjunction;
		result += *itBegin;
	}
	return result;
}


/**************************** erase_all ***************************/
// taken from https://stackoverflow.com/a/20326454

inline void erase_all(std::string& str, char c) {
	str.erase(std::remove(str.begin(), str.end(), c), str.end());
}

inline void erase_all(std::wstring& str, wchar_t c) {
	str.erase(std::remove(str.begin(), str.end(), c), str.end());
}

inline std::string erase_all_copy(std::string str, char c) {
	str.erase(std::remove(str.begin(), str.end(), c), str.end());
	return str;
}

inline std::wstring erase_all_copy(std::wstring str, wchar_t c) {
	str.erase(std::remove(str.begin(), str.end(), c), str.end());
	return str;
}


/**************************** case-conv ***************************/

inline void to_upper(std::string& str) {
	std::for_each(str.begin(), str.end(), [](char& c) { c = ::toupper(c); });
}

inline void to_upper(std::wstring& str) {
	std::for_each(str.begin(), str.end(), [](wchar_t& c) { c = ::toupper(c); });
}

inline std::string to_upper_copy(std::string str) {
	std::for_each(str.begin(), str.end(), [](char& c) { c = ::toupper(c); });
	return str;
}

inline std::wstring to_upper_copy(std::wstring str) {
	std::for_each(str.begin(), str.end(), [](wchar_t& c) { c = ::toupper(c); });
	return str;
}

inline void to_lower(std::string& str) {
	std::for_each(str.begin(), str.end(), [](char& c) { c = ::tolower(c); });
}

inline void to_lower(std::wstring& str) {
	std::for_each(str.begin(), str.end(), [](wchar_t& c) { c = ::tolower(c); });
}

inline std::string to_lower_copy(std::string str) {
	std::for_each(str.begin(), str.end(), [](char& c) { c = ::tolower(c); });
	return str;
}

inline std::wstring to_lower_copy(std::wstring str) {
	std::for_each(str.begin(), str.end(), [](wchar_t& c) { c = ::tolower(c); });
	return str;
}


/**************************** starts_with *************************/

inline bool starts_with(const std::string& str, const std::string& sub) {
	if (str.size() < sub.size()) { return false; }
	for (size_t i = 0; i < sub.size(); i++) { if (str[i] != sub[i]) { return false; } }
	return true;
}

inline bool starts_with(const std::wstring& str, const std::wstring& sub) {
	if (str.size() < sub.size()) { return false; }
	for (size_t i = 0; i < sub.size(); i++) { if (str[i] != sub[i]) { return false; } }
	return true;
}

/**************************** ends_with ***************************/

inline bool ends_with(const std::string& str, const std::string& sub) {
	if (str.size() < sub.size()) { return false; }
	for (size_t i = 0; i < sub.size(); i++) { 
		if (str[str.size() - i - 1] != sub[sub.size() - i - 1]) {
			return false;
		}
	}
	return true;
}

inline bool ends_with(const std::wstring& str, const std::wstring& sub) {
	if (str.size() < sub.size()) { return false; }
	for (size_t i = 0; i < sub.size(); i++) {
		if (str[str.size() - i - 1] != sub[sub.size() - i - 1]) {
			return false;
		}
	}
	return true;
}


} // namespace jlib
