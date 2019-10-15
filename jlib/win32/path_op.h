#pragma once

#include <algorithm>
#include <Windows.h>
#include <ShlObj.h>
#include <string>
#include <algorithm>

#pragma comment(lib, "Shell32.lib")

namespace jlib {
namespace win32 {

inline std::wstring getExePath()
{
	wchar_t path[1024] = { 0 };
	GetModuleFileNameW(nullptr, path, 1024);
	std::wstring::size_type pos = std::wstring(path).find_last_of(L"\\/");
	return std::wstring(path).substr(0, pos);
}

inline std::string getExePathA()
{
	char path[1024] = { 0 };
	GetModuleFileNameA(nullptr, path, 1024);
	std::string::size_type pos = std::string(path).find_last_of("\\/");
	return std::string(path).substr(0, pos);
}

static constexpr wchar_t* DEFAULT_PATH_FILTERW = L"\\/:*?\"<>| ";
static constexpr char* DEFAULT_PATH_FILTER = "\\/:*?\"<>| ";

inline std::wstring integratePath(const std::wstring& path, const std::wstring& filter = DEFAULT_PATH_FILTERW, wchar_t replace_by = L'_') {
	auto ret = path;
	for (auto c : filter) {
		std::replace(ret.begin(), ret.end(), c, replace_by);
	}
	return ret;
}

inline std::string integratePath(const std::string& path, const std::string& filter = DEFAULT_PATH_FILTER, char replace_by = '_') {
	auto ret = path;
	for (auto c : filter) {
		std::replace(ret.begin(), ret.end(), c, replace_by);
	}
	return ret;
}

inline std::wstring getSpecialFolder(int csidl) {
	wchar_t path[MAX_PATH] = { 0 };
	if (SHGetSpecialFolderPathW(nullptr, path, csidl, false)) {
		return std::wstring(path);
	}
	return std::wstring();
}

inline std::string getSpecialFolderA(int csidl) {
	char path[MAX_PATH] = { 0 };
	if (SHGetSpecialFolderPathA(nullptr, path, csidl, false)) {
		return std::string(path);
	}

	return std::string();
}

}
}
