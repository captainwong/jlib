#pragma once

#include <windows.h>
#include <string>

namespace jlib
{
namespace win32
{


//inline bool mbcs_to_u16(const char* mbcs, wchar_t* u16buffer, size_t u16size) {
//	size_t request_size = MultiByteToWideChar(CP_ACP, 0, mbcs, -1, NULL, 0);
//	if (0 < request_size && request_size < u16size) {
//		MultiByteToWideChar(CP_ACP, 0, mbcs, -1, u16buffer, request_size);
//		return true;
//	}
//	return false;
//};

inline std::wstring mbcs_to_utf16(const std::string& mbcs) {
	std::wstring res = L"";
	int request_size = MultiByteToWideChar(CP_ACP, 0, mbcs.c_str(), -1, NULL, 0);
	if (0 < request_size) {
		auto buff = new wchar_t[request_size];
		MultiByteToWideChar(CP_ACP, 0, mbcs.c_str(), -1, buff, request_size);
		res = buff;
		delete[] buff;
	}
	return res;
}

inline std::string utf16_to_mbcs(const std::wstring& u16) {
	std::string res = "";
	int request_size = WideCharToMultiByte(CP_ACP, 0, u16.c_str(), -1, 0, 0, 0, 0);
	if (0 < request_size) {
		auto buff = new char[request_size];
		WideCharToMultiByte(CP_ACP, 0, u16.c_str(), -1, buff, request_size, 0, 0);
		res = buff;
		delete[] buff;
	}
	return res;
}

inline std::string utf16_to_utf8(const std::wstring& u16) {
	std::string res = "";
	int request_size = WideCharToMultiByte(CP_UTF8, 0, u16.c_str(), -1, 0, 0, 0, 0);
	if (0 < request_size) {
		auto buff = new char[request_size];
		WideCharToMultiByte(CP_UTF8, 0, u16.c_str(), -1, buff, request_size, 0, 0);
		res = buff;
		delete[] buff;
	}
	return res;
}

inline std::string mbcs_to_utf8(const std::string& mbcs) {
	return utf16_to_utf8(mbcs_to_utf16(mbcs));
}

inline std::wstring utf8_to_utf16(const std::string u8) {
	std::wstring res = L"";
	int request_size = MultiByteToWideChar(CP_UTF8, 0, u8.c_str(), -1, NULL, 0);
	if (0 < request_size) {
		auto buff = new wchar_t[request_size];
		MultiByteToWideChar(CP_UTF8, 0, u8.c_str(), -1, buff, request_size);
		res = buff;
		delete[] buff;
	}
	return res;
}

inline std::string utf8_to_mbcs(const std::string& u8) {
	return utf16_to_mbcs(utf8_to_utf16(u8));
}


//
//
//inline bool Utf16ToUtf8(const wchar_t* utf16String, char* utf8Buffer, size_t szBuff)
//{
//	size_t request_size = WideCharToMultiByte(CP_UTF8, 0, utf16String, -1, NULL, 0, 0, 0);
//	if (1 < request_size && request_size < szBuff) {
//		WideCharToMultiByte(CP_UTF8, 0, utf16String, -1, utf8Buffer, request_size, 0, 0);
//		return true;
//	}
//	return false;
//}
//
//
//inline bool Utf8ToUtf16(const char* utf8String, wchar_t* utf16Buffer, size_t szBuff)
//{
//	size_t request_size = MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, NULL, 0);
//	if (1 < request_size && request_size < szBuff) {
//		MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, utf16Buffer, request_size);
//		return true;
//	}
//	return false;
//}
//
///**************UNICODE-ANSI mutually transform**************************/
//// need be manuly delete
//__forceinline LPSTR Utf16ToAnsi(const wchar_t* const wSrc)
//{
//	char* pElementText;
//	int iTextLen;
//	iTextLen = WideCharToMultiByte(CP_ACP, 0, wSrc, -1, NULL, 0, NULL, NULL);
//	pElementText = new char[iTextLen + 1];
//	memset((void*)pElementText, 0, sizeof(char) * (iTextLen + 1));
//	::WideCharToMultiByte(CP_ACP, 0, wSrc, -1, pElementText, iTextLen, NULL, NULL);
//
//	return pElementText;
//}
//
//__forceinline BOOL Utf16ToAnsiUseCharArray(const wchar_t* const wSrc,
//										   char* ansiArr, DWORD ansiArrLen)
//{
//	int iTextLen = WideCharToMultiByte(CP_ACP, 0, wSrc, -1, NULL, 0, NULL, NULL);
//	if (static_cast<DWORD>(iTextLen) < ansiArrLen) {
//		memset((void*)ansiArr, 0, sizeof(char) * (iTextLen + 1));
//		::WideCharToMultiByte(CP_ACP, 0, wSrc, -1, ansiArr, iTextLen, NULL, NULL);
//		return TRUE;
//	}
//	return FALSE;
//}
//
//__forceinline wchar_t* AnsiToUtf16(PCSTR ansiSrc)
//{
//	wchar_t* pWide;
//	int iUnicodeLen = ::MultiByteToWideChar(CP_ACP,
//											0, ansiSrc, -1, NULL, 0);
//	pWide = new wchar_t[iUnicodeLen + 1];
//	memset(pWide, 0, (iUnicodeLen + 1) * sizeof(wchar_t));
//	::MultiByteToWideChar(CP_ACP, 0, ansiSrc, -1, (LPWSTR)pWide, iUnicodeLen);
//	return pWide;
//}
//
//__forceinline BOOL AnsiToUtf16Array(PCSTR ansiSrc, wchar_t* warr, int warr_len)
//{
//	int iUnicodeLen = ::MultiByteToWideChar(CP_ACP,
//											0, ansiSrc, -1, NULL, 0);
//	if (warr_len >= iUnicodeLen) {
//		memset(warr, 0, (iUnicodeLen + 1) * sizeof(wchar_t));
//		::MultiByteToWideChar(CP_ACP, 0, ansiSrc, -1, (LPWSTR)warr, iUnicodeLen);
//		return TRUE;
//	}
//	return FALSE;
//}
//
//__forceinline wchar_t* Utf8ToUtf16(PCSTR ansiSrc)
//{
//	wchar_t* pWide;
//	int  iUnicodeLen = ::MultiByteToWideChar(CP_UTF8,
//											 0, ansiSrc, -1, NULL, 0);
//	pWide = new  wchar_t[iUnicodeLen + 1];
//	memset(pWide, 0, (iUnicodeLen + 1) * sizeof(wchar_t));
//	::MultiByteToWideChar(CP_UTF8, 0, ansiSrc, -1, (LPWSTR)pWide, iUnicodeLen);
//	return pWide;
//}
//
//
//__inline const char* Utf16ToUtf8(const wchar_t* utf16, int& out_len)
//{
//	out_len = ::WideCharToMultiByte(CP_UTF8, 0, utf16, -1, NULL, 0, 0, 0);
//	char* p8 = new  char[out_len + 1];
//	memset(p8, 0, (out_len + 1) * sizeof(char));
//	::WideCharToMultiByte(CP_UTF8, 0, utf16, -1, p8, out_len, 0, 0);
//	return p8;
//}

}
}
