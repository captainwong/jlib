#pragma once

#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#include "../utf8.h"

namespace jlib
{

namespace win32
{

/**
* @brief 启动程序
* @param path 启动命令
* @param wait_app_exit 是否阻塞等待程序退出
* @param show 是否显示程序
* @return 启动程序失败返回0xFFFFFFFF；当 wait_app_exit 为真，返回程序的进程返回值；否则返回程序进程ID；
*/
inline DWORD daemon(const std::wstring& path, bool wait_app_exit = true, bool show = true) {
	STARTUPINFOW si = { 0 };
	si.cb = sizeof(si);
	si.dwFlags |= STARTF_USESHOWWINDOW;
	si.wShowWindow = show ? SW_SHOW : SW_HIDE;
	PROCESS_INFORMATION pi;
	DWORD dwCreationFlags = show ? 0 : CREATE_NO_WINDOW;
	BOOL bRet = CreateProcessW(NULL, (LPWSTR)(path.c_str()), NULL, NULL, FALSE, dwCreationFlags, NULL, NULL, &si, &pi);
	if (bRet) {
		if (wait_app_exit) {
			WaitForSingleObject(pi.hProcess, INFINITE);
			DWORD dwExit;
			::GetExitCodeProcess(pi.hProcess, &dwExit);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			return dwExit;
		} else {
			return pi.dwProcessId;
		}
	}
	return 0xFFFFFFFF;
}

/**
* @brief 启动程序
* @param path 启动命令
* @param wait_app_exit 是否阻塞等待程序退出
* @param show 是否显示程序
* @return 启动程序失败返回0xFFFFFFFF；当 wait_app_exit 为真，返回程序的进程返回值；否则返回程序进程ID；
*/
inline DWORD daemon(const std::string& path, bool wait_app_exit = true, bool show = true) {
	return daemon(utf8::a2w(path), wait_app_exit, show);
}

/**
* @brief 获取父进程ID
* @return 成功返回父进程ID，否则返回0
*/
inline DWORD getppid()
{
	HANDLE hSnapshot;
	PROCESSENTRY32 pe32;
	DWORD ppid = 0, pid = GetCurrentProcessId();

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	__try {
		if (hSnapshot == INVALID_HANDLE_VALUE) __leave;

		ZeroMemory(&pe32, sizeof(pe32));
		pe32.dwSize = sizeof(pe32);
		if (!Process32First(hSnapshot, &pe32)) __leave;

		do {
			if (pe32.th32ProcessID == pid) {
				ppid = pe32.th32ParentProcessID;
				break;
			}
		} while (Process32Next(hSnapshot, &pe32));

	} __finally {
		if (hSnapshot != INVALID_HANDLE_VALUE) CloseHandle(hSnapshot);
	}
	return ppid;
}


} // namespace win32

} // namespace jlib
