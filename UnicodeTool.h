#pragma once

#include <windows.h>

bool Utf16ToUtf8(const wchar_t* utf16String, char* utf8Buffer, size_t szBuff)
{
	size_t request_size = WideCharToMultiByte(CP_UTF8, 0, utf16String, -1, NULL, 0, 0, 0);
	if (1 < request_size && request_size < szBuff) {
		WideCharToMultiByte(CP_UTF8, 0, utf16String, -1, utf8Buffer, request_size, 0, 0);
		return true;
	}
	return false;
}


bool Utf8ToUtf16(const char* utf8String, wchar_t* utf16Buffer, size_t szBuff)
{
	size_t request_size = MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, NULL, 0);
	if (1 < request_size && request_size < szBuff) {
		MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, utf16Buffer, request_size);
		return true;
	}
	return false;
}