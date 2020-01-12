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


//************ string **************//

// trim from start (in place)
inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch); }));
}

// trim from end (in place)
inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch); }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string& s) {
	ltrim(s); rtrim(s);
}

// trim from start (copying)
inline std::string ltrim_copy(std::string s) {
	ltrim(s); return s;
}

// trim from end (copying)
inline std::string rtrim_copy(std::string s) {
	rtrim(s); return s;
}

// trim from both ends (copying)
inline std::string trim_copy(std::string s) {
	trim(s); return s;
}


//************ wstring **************//

// trim from start (in place)
inline void ltrim(std::wstring& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch); }));
}

// trim from end (in place)
inline void rtrim(std::wstring& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch); }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::wstring& s) {
	ltrim(s); rtrim(s);
}

// trim from start (copying)
inline std::wstring ltrim_copy(std::wstring s) {
	ltrim(s); return s;
}

// trim from end (copying)
inline std::wstring rtrim_copy(std::wstring s) {
	rtrim(s); return s;
}

// trim from both ends (copying)
inline std::wstring trim_copy(std::wstring s) {
	trim(s); return s;
}


/**************************** join ***************************/

/**
* @brief join 字符串
* @note StringType 可以为std::string或std::wstring
* @note StringContainer 必须为上述StringType的iterable容器类，如vector，list
*/
template <typename StringType, typename StringContainer>
StringType join(const StringContainer& container, const StringType& conjunction = StringType())
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

} // namespace jlib
