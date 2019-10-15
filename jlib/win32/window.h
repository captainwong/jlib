#pragma once

#include <Windows.h>
#include "../base/noncopyable.h"

namespace jlib
{

namespace win32
{

inline BOOL centerWindow(HWND hwnd)
{
	HWND hwndParent = GetParent(hwnd);
	if (hwndParent == NULL) {
		hwndParent = GetDesktopWindow();
	}

	// make the window relative to its parent
	if (hwndParent != NULL) {
		RECT rectWindow, rectParent;
		GetWindowRect(hwnd, &rectWindow);
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

		MoveWindow(hwnd, nX, nY, nWidth, nHeight, FALSE);

		return TRUE;
	}

	return FALSE;
}


class AutoReopenTimer : public noncopyable
{
private:
	HWND hWnd_ = NULL;
	int timerId_ = 0;
	DWORD timeOut_ = 0;

public:
	explicit AutoReopenTimer(HWND hWnd, int timerId, DWORD timeout) : hWnd_(hWnd), timerId_(timerId), timeOut_(timeout)
	{
		KillTimer(hWnd, timerId_);
	}

	~AutoReopenTimer()
	{
		SetTimer(hWnd_, timerId_, timeOut_, NULL);
	}
};

}
}
