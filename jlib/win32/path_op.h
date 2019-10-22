#pragma once

#include <algorithm>
#include <Windows.h>
#include <ShlObj.h>
#include <string>
#include <algorithm>

#pragma comment(lib, "Shell32.lib")

namespace jlib {
namespace win32 {

inline std::wstring getExePath() {
	wchar_t path[1024] = { 0 }; ::GetModuleFileNameW(nullptr, path, 1024); return path;
}

inline std::string getExePathA() {
	char path[1024] = { 0 }; ::GetModuleFileNameA(nullptr, path, 1024); return path;
}

inline std::wstring getExeFolderPath() {
	auto path = getExePath(); auto pos = path.find_last_of(L"\\/"); return path.substr(0, pos);
}

inline std::string getExeFolderPathA() {
	auto path = getExePathA(); auto pos = path.find_last_of("\\/"); return path.substr(0, pos);
}

static constexpr const wchar_t* DEFAULT_PATH_FILTERW = L"\\/:*?\"<>| ";
static constexpr const char* DEFAULT_PATH_FILTER = "\\/:*?\"<>| ";

inline std::wstring integrateFileName(const std::wstring& name, const std::wstring& filter = DEFAULT_PATH_FILTERW, wchar_t replace_by = L'_') {
	auto ret = name; for (auto c : filter) { std::replace(ret.begin(), ret.end(), c, replace_by); } return ret;
}

inline std::string integrateFileName(const std::string& name, const std::string& filter = DEFAULT_PATH_FILTER, char replace_by = '_') {
	auto ret = name; for (auto c : filter) { std::replace(ret.begin(), ret.end(), c, replace_by); } return ret;
}

inline std::wstring getSpecialFolder(int csidl) {
	wchar_t path[MAX_PATH] = { 0 }; 
	return ::SHGetSpecialFolderPathW(nullptr, path, csidl, false) ? std::wstring(path) : std::wstring();
}

inline std::string getSpecialFolderA(int csidl) {
	char path[MAX_PATH] = { 0 }; 
	return ::SHGetSpecialFolderPathA(nullptr, path, csidl, false) ? std::string(path) : std::string();
}

/**
* @brief 获取%APPDATA%路径，一般用于存储程序配置文件
* @note 一般xp的结果为： C:\Documents and Setting\[UserName]\Application Data
* @note 一般vista及以上的结果为：C:\Users\[UserName]\AppData\Roaming
*/
inline std::wstring getAppDataPath() { return getSpecialFolder(CSIDL_APPDATA); }
inline std::string getAppDataPathA() { return getSpecialFolderA(CSIDL_APPDATA); }

inline std::wstring getTempPath() { wchar_t path[MAX_PATH] = { 0 }; ::GetTempPathW(MAX_PATH, path); return path; }
inline std::string getTempPathA() { char path[MAX_PATH] = { 0 }; ::GetTempPathA(MAX_PATH, path); return path; }

inline std::wstring getTempFileName(const std::wstring& folder, const std::wstring& pre) {
	wchar_t path[MAX_PATH] = { 0 };
	UINT ret = ::GetTempFileNameW(folder.c_str(), pre.c_str(), 0, path);
	if (ret != 0) { return path; } return std::wstring();
}

inline std::string getTempFileName(const std::string& folder, const std::string& pre) {
	char path[MAX_PATH] = { 0 };
	UINT ret = ::GetTempFileNameA(folder.c_str(), pre.c_str(), 0, path);
	if (ret != 0) { return path; } return std::string();
}

inline std::wstring getTempFileName(const std::wstring& pre = L"JLIB") {
	return getTempFileName(getTempPath(), pre);
}

inline std::string getTempFileNameA(const std::string& pre = "JLIB") {
	return getTempFileName(getTempPathA(), pre);
}

}
}
