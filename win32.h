#pragma once

#include <windows.h>
#include <string>
#include "utf8.h"

namespace utf8 {

inline bool mbcs_to_u16(const char* mbcs, wchar_t* u16buffer, size_t u16size) {
	size_t request_size = MultiByteToWideChar(CP_ACP, 0, mbcs, -1, NULL, 0);
	if (0 < request_size && request_size < u16size) {
		MultiByteToWideChar(CP_ACP, 0, mbcs, -1, u16buffer, request_size);
		return true;
	}
	return false;
};

inline std::wstring mbcs_to_u16(const std::string& mbcs) {
	std::wstring res = L"";
	size_t request_size = MultiByteToWideChar(CP_ACP, 0, mbcs.c_str(), -1, NULL, 0);
	if (0 < request_size) {
		auto u16buffer = new wchar_t[request_size];
		MultiByteToWideChar(CP_ACP, 0, mbcs.c_str(), -1, u16buffer, request_size);
		res = u16buffer;
		delete[] u16buffer;
	}
	return res;
}

inline std::string u16_to_mbcs(const std::wstring& u16) {
	std::string res = "";
	size_t request_size = WideCharToMultiByte(CP_ACP, 0, u16.c_str(), -1, 0, 0, 0, 0);
	if (0 < request_size) {
		auto mbcs_buffer = new char[request_size];
		WideCharToMultiByte(CP_ACP, 0, u16.c_str(), -1, mbcs_buffer, request_size, 0, 0);
		res = mbcs_buffer;
		delete[] mbcs_buffer;
	}
	return res;
}


};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

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

inline BOOL Win32CenterWindow(HWND hwndWindow)
{
	HWND hwndParent;
	RECT rectWindow, rectParent;

	if ((hwndParent = GetParent(hwndWindow)) == NULL) {
		hwndParent = GetDesktopWindow();
	}

	// make the window relative to its parent
	if (hwndParent != NULL) {
		GetWindowRect(hwndWindow, &rectWindow);
		GetWindowRect(hwndParent, &rectParent);

		int nWidth = rectWindow.right - rectWindow.left;
		int nHeight = rectWindow.bottom - rectWindow.top;

		int nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;
		int nY = ((rectParent.bottom - rectParent.top) - nHeight) / 2 + rectParent.top;

		int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		// make sure that the dialog box never moves outside of the screen
		if (nX < 0) nX = 0;
		if (nY < 0) nY = 0;
		if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
		if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;

		MoveWindow(hwndWindow, nX, nY, nWidth, nHeight, FALSE);

		return TRUE;
	}

	return FALSE;
}

class auto_timer : public boost::noncopyable
{
private:
	int m_timer_id;
	DWORD m_time_out;
	HWND m_hWnd;

public:

	auto_timer(HWND hWnd, int timerId, DWORD timeout) : m_hWnd(hWnd), m_timer_id(timerId), m_time_out(timeout)
	{
		KillTimer(hWnd, m_timer_id);
	}

	~auto_timer()
	{
		SetTimer(m_hWnd, m_timer_id, m_time_out, nullptr);
	}
};



};