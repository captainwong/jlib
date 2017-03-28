#pragma once
#include <windows.h>
#include <string>

namespace jlib{

inline bool toClipboard(HWND hwnd, const std::string &s) 
{
    if (!OpenClipboard(hwnd)) return false;
    if (!EmptyClipboard()) return false;
    HGLOBAL hg = GlobalAlloc(GMEM_DDESHARE, s.size() + 1);
    if (!hg) {
        CloseClipboard();
        return false;
    }
    LPVOID hMem = GlobalLock(hg);
    if (!hMem) {
        CloseClipboard();
        return false;
    }
    memcpy(hMem, s.c_str(), s.size() + 1);
    GlobalUnlock(hg);
    SetClipboardData(CF_TEXT, hg);
    CloseClipboard();
    GlobalFree(hg);
    return true;
}
    
    
}