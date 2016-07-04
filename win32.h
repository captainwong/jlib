#pragma once

#include <boost/noncopyable.hpp>
#include <windows.h>
#include <string>
#include <vector>
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


inline std::string mbcs_to_utf8(const std::wstring& mbcs) {
	std::string res = "";
	size_t request_size = WideCharToMultiByte(CP_UTF8, 0, mbcs.c_str(), -1, 0, 0, 0, 0);
	if (0 < request_size) {
		auto mbcs_buffer = new char[request_size];
		WideCharToMultiByte(CP_UTF8, 0, mbcs.c_str(), -1, mbcs_buffer, request_size, 0, 0);
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


namespace rc_detail {

inline auto Width(LPCRECT rc) {
	return rc->right - rc->left;
}

inline auto Height(LPCRECT rc) {
	return rc->bottom - rc->top;
}

inline void DeflateRect(LPRECT rc, int l, int t, int r, int b) {
	rc->left += l;
	rc->top += t;
	rc->right -= r;
	rc->bottom -= b;
}

inline void InflateRect(LPRECT rc, int l, int t, int r, int b) {
	rc->left -= l;
	rc->top -= t;
	rc->right += r;
	rc->bottom += b;
}

}

// 将矩形平均分割成n份,间距2*gap, n is x^2, x={1,2,3...}
inline std::vector<RECT> split_rect(LPCRECT rc, int n, int gap = 50) {
	using namespace rc_detail;

	std::vector<RECT> v;
	for (int i = 0; i < n; i++) {
		v.push_back(*rc);
	}

	double l = sqrt(n);
	int line = int(l);

	int col_step = (int)(Width(rc) / line);
	int row_step = (int)(Height(rc) / line);

	for (int i = 0; i < n; i++) {
		v[i].left = rc->left + (i % line) * col_step;
		v[i].right = v[i].left + col_step;
		v[i].top = rc->top + (i / line) * row_step;
		v[i].bottom = v[i].top + row_step;
		DeflateRect(&v[i], gap, gap, gap, gap);
	}

	return v;
};

// 将矩形水平平均分割为n份矩形, 当hgap==-1时，分割出的矩形与源矩形保持比例
inline std::vector<RECT> split_rect_horizontal(LPCRECT rc, int n, int wgap = 50, int hgap = -1) {
	using namespace rc_detail;
	std::vector<RECT> v;
	
	int w = (Width(rc) - (n + 1) * wgap) / n;

	if (hgap == -1) {
		double ratio = (rc->right - rc->left) * 1.0 / (rc->bottom - rc->top);
		int h = static_cast<int>(w / ratio);
		hgap = (Height(rc) - h) / 2;
	}

	for (int i = 0; i < n; i++) {
		RECT r = *rc;
		r.left += i*w + (i + 1)*wgap;
		r.right = r.left + w;
		r.top = rc->top + hgap;
		r.bottom = rc->bottom - hgap;
		v.push_back(r);
	}
	
	return v;
}

// rc's width / spliter = (rc's width - hexagon's side length) / 2
inline std::vector<POINT> get_hexagon_vertexes_from_rect(LPCRECT rc, float spliter = 3.0) {
	if (!rc) {
		return std::vector<POINT>();
	}

	if (spliter == 0.0) {
		spliter = 3.0;
	}

	std::vector<POINT> v;
	auto w = rc->right - rc->left;
	auto h = rc->bottom - rc->top;
	auto ww = static_cast<int>(w / spliter);
	auto hh = static_cast<int>(h / spliter);

	POINT pt;
	pt.x = rc->left;
	pt.y = rc->top + hh;
	v.push_back(pt);

	pt.x = rc->left + ww;
	pt.y = rc->top;
	v.push_back(pt);

	pt.x = rc->right - ww;
	v.push_back(pt);

	pt.x = rc->right;
	pt.y = rc->top + hh;
	v.push_back(pt);

	pt.y = rc->bottom - hh;
	v.push_back(pt);

	pt.x = rc->right - ww;
	pt.y = rc->bottom;
	v.push_back(pt);

	pt.x = rc->left + ww;
	v.push_back(pt);

	pt.x = rc->left;
	pt.y = rc->bottom - hh;
	v.push_back(pt);

	return v;
}

};