#pragma once
#include <windows.h>
#include <string>

namespace jlib{

inline bool to_clipboard(const std::wstring &s)
{
	if (!OpenClipboard(nullptr)) return false;
	if (!EmptyClipboard()) return false;
	size_t sz = (s.length() + 1) * 2;
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, sz);
	if (!hg) {
		CloseClipboard();
		return false;
	}
	LPVOID hMem = GlobalLock(hg);
	if (!hMem) {
		CloseClipboard();
		return false;
	}
	memset(hMem, 0, sz);
	memcpy(hMem, s.data(), s.size() * 2);
	GlobalUnlock(hg);
	SetClipboardData(CF_UNICODETEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
	return true;
}

inline std::wstring from_clipboard()
{
	std::wstring text;
	do {
		if (!OpenClipboard(nullptr))
			break;

		HANDLE hData = GetClipboardData(CF_UNICODETEXT);
		if (hData == nullptr)
			break;

		auto pszText = static_cast<const wchar_t*>(GlobalLock(hData));
		if (pszText == nullptr)
			break;

		text = pszText;
		GlobalUnlock(hData);
		CloseClipboard();
	} while (false);
	return text;
}

    
}