#pragma once

#include <string>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <vector>


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
* @note StringType 可以为 std::string 或 std::wstring
* @note StringContainer 必须为上述 StringType的iterable 容器类，如vector，list
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


/**************************** split ***************************/
/**
* @brief split 字符串
* @note StringType 可以为 std::string 或 std::wstring
* @note StringContainer 必须为上述StringType的iterable容器类，如vector，list
*/
template <typename StringType = std::string, typename StringContainer = typename std::vector<StringType>>
inline StringContainer split(const StringType& str, const StringType& split_by)
{
	StringContainer result;
	if (str.size() <= split_by.size()) {
		return result;
	} else if (split_by.empty()) {
		if (!str.empty()) {
			result.push_back(str);
		}
		return result;
	}
	using size_type = typename StringType::size_type;
	size_type pos = 0, spos = 0;
	while (pos < str.size()) {
		spos = str.find(split_by, pos);
		if (spos != StringType::npos) {
			if (spos > pos) {
				result.push_back(str.substr(pos, spos - pos));
			}
			pos = spos + split_by.size();
		} else if (pos < str.size()) {
			result.push_back(str.substr(pos, str.size() - pos));
			break;
		}
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
	std::for_each(str.begin(), str.end(), [](char& c) { c = (char)::toupper(c); });
}

inline void to_upper(std::wstring& str) {
	std::for_each(str.begin(), str.end(), [](wchar_t& c) { c = (char)::toupper(c); });
}

inline std::string to_upper_copy(std::string str) {
	std::for_each(str.begin(), str.end(), [](char& c) { c = (char)::toupper(c); });
	return str;
}

inline std::wstring to_upper_copy(std::wstring str) {
	std::for_each(str.begin(), str.end(), [](wchar_t& c) { c = (char)::toupper(c); });
	return str;
}

inline void to_lower(std::string& str) {
	std::for_each(str.begin(), str.end(), [](char& c) { c = (char)::tolower(c); });
}

inline void to_lower(std::wstring& str) {
	std::for_each(str.begin(), str.end(), [](wchar_t& c) { c = (char)::tolower(c); });
}

inline std::string to_lower_copy(std::string str) {
	std::for_each(str.begin(), str.end(), [](char& c) { c = (char)::tolower(c); });
	return str;
}

inline std::wstring to_lower_copy(std::wstring str) {
	std::for_each(str.begin(), str.end(), [](wchar_t& c) { c = (char)::tolower(c); });
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


/**************************** justify ***************************/
/*
These functions respectively left-justify, right-justify and center a string in a field of given width. 
They return a string that is at least width characters wide, created by padding the string s with the 
	character fillchar (default is a space) until the given width on the right, left or both sides. 
The string is never truncated.
*/

inline std::string ljust(const std::string& str, size_t width, char fillchar = ' ') {
	if (str.size() >= width) { return str; }
	auto s = str;
	while (s.size() < width) { s.push_back(fillchar); }
	return s;
}

inline std::string rjust(const std::string& str, size_t width, char fillchar = ' ') {
	if (str.size() >= width) { return str; }
	auto s = str;
	while (s.size() < width) { s.insert(s.begin(), fillchar); }
	return s;
}

inline std::string center(const std::string& str, size_t width, char fillchar = ' ') {
	if (str.size() >= width) { return str; }
	auto s = str;
	while (s.size() < width) { s.insert(s.begin(), fillchar); s.push_back(fillchar); }
	if (s.size() > width) { s.pop_back(); }
	return s;
}

inline std::wstring ljust(const std::wstring& str, size_t width, wchar_t fillchar = L' ') {
	if (str.size() >= width) { return str; }
	auto s = str;
	while (s.size() < width) { s.push_back(fillchar); }
	return s;
}

inline std::wstring rjust(const std::wstring& str, size_t width, wchar_t fillchar = L' ') {
	if (str.size() >= width) { return str; }
	auto s = str;
	while (s.size() < width) { s.insert(s.begin(), fillchar); }
	return s;
}

inline std::wstring center(const std::wstring& str, size_t width, wchar_t fillchar = L' ') {
	if (str.size() >= width) { return str; }
	auto s = str;
	while (s.size() < width) { s.insert(s.begin(), fillchar); s.push_back(fillchar); }
	if (s.size() > width) { s.pop_back(); }
	return s;
}


/**
* @brief 将范围为0~15的十进制数字字节转换为16进制字节
* @param d 要计算的10进制数字，范围0~15
* @return 十六进制字节，大写
* @note 示例: 数字10 返回字节 'A'
*/
inline char Dec2Hex(char d)
{
	if (0 <= d && d <= 9) {
		return static_cast<char>((unsigned char)d + (unsigned char)'0');
	} else if (0x0A <= d && d <= 0x0F) {
		return char(d - 0x0A + (char)'A');
	} else {
		return '0';
	}
}


enum class ToStringOption
{
	//! 所有字节按照16进制表示，如 '0' 表示为 "\x30"
	ALL_CHAR_AS_HEX,
	//! 如果字节是可打印的直接输出，否则输出16进制格式
	TRY_IS_PRINT_FIRST,
};

/**
* @brief 常规数组转字符串
* @param data 数据
* @param len 数据长度
* @param option 选项
* @param show_x_for_hex 为 hex 显示 \x
* @param show_space_between_hex 在 hex 之间插入空格
* @return std::string
*/
inline std::string toString(const char* data, size_t len, // 数据
							ToStringOption option = ToStringOption::TRY_IS_PRINT_FIRST, // 选项
							bool show_x_for_hex = true,	// 为 hex 显示 \x
							bool show_space_between_hex = false) // 在 hex 之间插入空格
{
	std::string str;
	for (size_t i = 0; i < len; i++) {
		auto c = data[i];
		if (option == ToStringOption::TRY_IS_PRINT_FIRST && std::isprint(static_cast<unsigned char>(c))) {
			str.push_back(c);
		} else {
			if (show_x_for_hex) { str += "\\x"; }
			str.push_back(Dec2Hex((c >> 4) & 0x0F));
			str.push_back(Dec2Hex(c & 0x0F));
			if (show_space_between_hex) { str.push_back(' '); }
		}
	}
	return str;
}

inline std::string toString(const unsigned char* data, size_t len, ToStringOption option = ToStringOption::TRY_IS_PRINT_FIRST, bool show_x_for_hex = true, bool show_space_between_hex = false)
{
	return toString((const char*)data, len, option, show_x_for_hex, show_space_between_hex);
}

} // namespace jlib
