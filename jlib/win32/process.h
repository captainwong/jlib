#pragma once

#include <Windows.h>
#include <string>
#include "../utf8.h"

namespace jlib
{

namespace win32
{

inline DWORD daemon(const std::wstring& path, bool wait_app_exit = true, bool show = true) {
	STARTUPINFOW si = { 0 };
	si.cb = sizeof(si);
	si.dwFlags |= STARTF_USESHOWWINDOW;
	si.wShowWindow = show ? SW_SHOW : SW_HIDE;
	PROCESS_INFORMATION pi;
	DWORD dwCreationFlags = show ? 0 : CREATE_NO_WINDOW;
	BOOL bRet = CreateProcessW(NULL, (LPWSTR)(path.c_str()), NULL, NULL, FALSE, dwCreationFlags, NULL, NULL, &si, &pi);
	if (bRet) {
		WaitForSingleObject(pi.hProcess, wait_app_exit ? INFINITE : 0);
		DWORD dwExit;
		::GetExitCodeProcess(pi.hProcess, &dwExit);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return dwExit;
	}
	return 0xFFFFFFFF;
}

inline DWORD daemon(const std::string& path, bool wait_app_exit = true, bool show = true) {
	return daemon(utf8::a2w(path), wait_app_exit, show);
}

} // namespace win32

} // namespace jlib
