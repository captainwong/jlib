#pragma once
#include <string>
#include <algorithm>

namespace jlib {

inline std::wstring get_exe_path()
{
	wchar_t path[1024] = { 0 };
	GetModuleFileName(nullptr, path, 1024);
	std::wstring::size_type pos = std::wstring(path).find_last_of(L"\\/");
	return std::wstring(path).substr(0, pos);
}

inline std::string get_exe_path_a()
{
	char path[1024] = { 0 };
	GetModuleFileNameA(nullptr, path, 1024);
	std::string::size_type pos = std::string(path).find_last_of("\\/");
	return std::string(path).substr(0, pos);
}

inline std::wstring integrate_path(const std::wstring& path, wchar_t replace_by = L'_'){
    static const wchar_t filter[] = L"\\/:*?\"<>| ";
    auto ret = path;
    for (auto c : filter) {
        std::replace(ret.begin(), ret.end(), c, replace_by);
    }
    return ret;
}

inline std::string integrate_path(const std::string& path, char replace_by = '_'){
    static const char filter[] = "\\/:*?\"<>| ";
    auto ret = path;
    for (auto c : filter) {
        std::replace(ret.begin(), ret.end(), c, replace_by);
    }
    return ret;
}


}

