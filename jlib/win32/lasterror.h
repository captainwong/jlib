#pragma once

#include <Windows.h>
#include <string>

namespace jlib {
namespace win32 {

std::string formatLastError(const std::string& msg)
{
    // Get system msg
    char sysMsg[256];
    DWORD eNum = GetLastError();
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL, eNum,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                  sysMsg, 256, NULL);

    // Trim the end of the line and terminate it with a null
    char* p = sysMsg;
    while ((*p > 31) || (*p == 9)) ++p;
    do { *p-- = 0; } while ((p >= sysMsg) && ((*p == '.') || (*p < 33)));

    return msg + " failed with error " + std::to_string(eNum) + " (" + sysMsg + ")";
}

std::wstring formatLastError(const std::wstring& msg)
{
    // Get system msg
    wchar_t sysMsg[256];
    DWORD eNum = GetLastError();
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, eNum,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                   sysMsg, 256, NULL);

    // Trim the end of the line and terminate it with a null
    wchar_t* p = sysMsg;
    while ((*p > 31) || (*p == 9)) ++p;
    do { *p-- = 0; } while ((p >= sysMsg) && ((*p == '.') || (*p < 33)));

    return msg + L" failed with error " + std::to_wstring(eNum) + L" (" + sysMsg + L")";
}

}
}
