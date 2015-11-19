#pragma once
#pragma comment(lib, "USER32" )
#include <crtdbg.h>
#include <tchar.h>
#pragma comment(lib, "version.lib")
#define MTVERIFY(a) (a)
#if 0
#define SHOWERROR(a) PrintError(a,__FILE__,__LINE__,GetLastError())

#ifdef DEBUG
#define MTASSERT(a) _ASSERTE(a)
#define MTVERIFY(a) if (!(a)) PrintError(#a,__FILE__,__LINE__,GetLastError())
#else /*DEBUG*/
#define MTASSERT(a)
#define MTVERIFY(a) (a)
#endif /*DEBUG*/

__inline void PrintError(LPCTSTR linedesc, LPCTSTR filename, int lineno, DWORD errnum)
{
	LPTSTR lpBuffer;
	TCHAR errbuf[256];

#ifdef _WINDOWS_
	TCHAR modulename[MAX_PATH];
#else //_WINDOWS_
	DWORD numread;
#endif //_WINDOWS_

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		errnum,
		LANG_NEUTRAL,
		(LPTSTR)&lpBuffer,
		0,
		NULL
		);
	_stprintf(errbuf, _T("\nThe following call failed at line %d in %s:\n\n %s\n\nReason: %s\n"), 
			  lineno, filename, linedesc, lpBuffer);

#ifndef _WINDOWS_
	WriteFile(GetStdHandle(STD_ERROR_HANDLE), errbuf, lstrlen(errbuf), &numread, FALSE );
	Sleep(3000);
#else //_WINDOWS_
	GetModuleFileName(NULL, modulename, MAX_PATH);
	MessageBox(NULL, errbuf, modulename, MB_ICONWARNING|MB_OK|MB_TASKMODAL|MB_SETFOREGROUND);
	OutputDebugString(errbuf);
#endif //_WINDOWS_

	LocalFree(lpBuffer);
	//exit(EXIT_FAILURE);
}
#endif



__inline void WaitTillThreadActive(HANDLE &handle)
{
	if (handle != INVALID_HANDLE_VALUE) {
		BOOL rc = FALSE;
		DWORD ec = 0;
		for (;;) {
			rc = GetExitCodeThread(handle, &ec);
			if (rc && (ec == STILL_ACTIVE))
				break;
			Sleep(500);
		}
	}
}
// obsolete
/*
__inline void WaitTillThreadExited(HANDLE &handle, DWORD dwMilliseconds = 3000)
{
if(handle != INVALID_HANDLE_VALUE)
{
BOOL rc = FALSE;
DWORD ec = 0;
DWORD dwStart = GetTickCount();
for(;;)
{
if(GetTickCount() - dwStart >= dwMilliseconds)
{
::TerminateThread(handle, 0xffffffff);
CloseHandle(handle);
handle = INVALID_HANDLE_VALUE;
break;
}
if(handle == INVALID_HANDLE_VALUE)
break;
rc = GetExitCodeThread(handle, &ec);
if(rc && (ec != STILL_ACTIVE))
{
CloseHandle(handle);
handle = INVALID_HANDLE_VALUE;
break;
}
Sleep(500);
}
}
}
*/

__forceinline void WaitTillThreadExited(HANDLE &handle, DWORD dwMilliseconds = 5000)
{
	if (handle == INVALID_HANDLE_VALUE)
		return;
	DWORD ret = ::WaitForSingleObject(handle, dwMilliseconds);
	if (ret != WAIT_OBJECT_0) {	//::TerminateThread(handle, 0xffffffff);
	}
	CloseHandle(handle);
	handle = INVALID_HANDLE_VALUE;
}

__inline LPTSTR GetModuleFilePath()
{
	static TCHAR szPath[MAX_PATH] = { '\0' };
	if (lstrlen(szPath) != 0)
		return szPath;

	TCHAR szExe[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL), szExe, MAX_PATH);
	int len = lstrlen(szExe);
	for (int i = len; i > 0; i--) {
		if (szExe[i] == '\\') {
			szExe[i] = '\0';
			lstrcpy(szPath, szExe);
			break;
		}
	}
	return szPath;
}

/////////*********************自定义宏********************////////////////////////
//安全删除普通堆内存
#define SAFEDELETEP(p) {if(p){delete (p); (p)=NULL;}}

#define SAFEDELETEARR(pArr) {if((pArr)){delete[] (pArr); (pArr)=NULL;}}

//安全删除对话框类堆内存
#define SAFEDELETEDLG(pDlg) {\
	if ((pDlg)) {\
		if (::IsWindow(pDlg->m_hWnd)) {	\
			(pDlg)->DestroyWindow(); \
		}\
		delete (pDlg); \
		(pDlg) = NULL; \
	}\
}

//关闭核心对象句柄
#define CLOSEHANDLE(h){\
	if (h != INVALID_HANDLE_VALUE && h != NULL) {\
		::CloseHandle(h); \
		h = INVALID_HANDLE_VALUE; \
	}\
}

#define CLOSESOCKET(s){\
	if (s != INVALID_SOCKET) {	\
		::closesocket(s); \
		s = INVALID_SOCKET; \
	}\
}

#ifdef _DEBUG
#define VERIFYPTR(p, cb) {MTVERIFY(!IsBadWritePtr(p, cb));}
#else
#define VERIFYPTR(p, cb)
#endif


/**************UNICODE-ANSI mutually transform**************************/
// need be manuly delete
__forceinline LPSTR Utf16ToAnsi(const wchar_t * const wSrc)
{
	char* pElementText;
	int iTextLen;
	iTextLen = WideCharToMultiByte(CP_ACP, 0, wSrc, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char)* (iTextLen + 1));
	::WideCharToMultiByte(CP_ACP, 0, wSrc, -1, pElementText, iTextLen, NULL, NULL);

	return pElementText;
}

__forceinline BOOL Utf16ToAnsiUseCharArray(const wchar_t * const wSrc,
										   char *ansiArr, DWORD ansiArrLen)
{
	int iTextLen = WideCharToMultiByte(CP_ACP, 0, wSrc, -1, NULL, 0, NULL, NULL);
	if (static_cast<DWORD>(iTextLen) < ansiArrLen) {
		memset((void*)ansiArr, 0, sizeof(char)* (iTextLen + 1));
		::WideCharToMultiByte(CP_ACP, 0, wSrc, -1, ansiArr, iTextLen, NULL, NULL);
		return TRUE;
	}
	return FALSE;
}

__forceinline wchar_t* AnsiToUtf16(PCSTR ansiSrc)
{
	wchar_t *pWide;
	int iUnicodeLen = ::MultiByteToWideChar(CP_ACP,
											 0, ansiSrc, -1, NULL, 0);
	pWide = new wchar_t[iUnicodeLen + 1];
	memset(pWide, 0, (iUnicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, ansiSrc, -1, (LPWSTR)pWide, iUnicodeLen);
	return pWide;
}

__forceinline BOOL AnsiToUtf16Array(PCSTR ansiSrc, wchar_t* warr, int warr_len)
{
	int iUnicodeLen = ::MultiByteToWideChar(CP_ACP,
											 0, ansiSrc, -1, NULL, 0);
	if (warr_len >= iUnicodeLen) {
		memset(warr, 0, (iUnicodeLen + 1) * sizeof(wchar_t));
		::MultiByteToWideChar(CP_ACP, 0, ansiSrc, -1, (LPWSTR)warr, iUnicodeLen);
		return TRUE;
	}
	return FALSE;
}

__forceinline wchar_t* Utf8ToUtf16(PCSTR ansiSrc)
{
	wchar_t *pWide;
	int  iUnicodeLen = ::MultiByteToWideChar(CP_UTF8,
											 0, ansiSrc, -1, NULL, 0);
	pWide = new  wchar_t[iUnicodeLen + 1];
	memset(pWide, 0, (iUnicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_UTF8, 0, ansiSrc, -1, (LPWSTR)pWide, iUnicodeLen);
	return pWide;
}


__inline const char* Utf16ToUtf8(const wchar_t* utf16, int& out_len)
{
	out_len = ::WideCharToMultiByte(CP_UTF8, 0, utf16, -1, NULL, 0, 0, 0);
	char *p8 = new  char[out_len + 1];
	memset(p8, 0, (out_len + 1) * sizeof(wchar_t));
	::WideCharToMultiByte(CP_UTF8, 0, utf16, -1, p8, out_len, 0, 0);
	return p8;
}


//__inline const char* Utf16ToGbk(const wchar_t* utf16)
//{
//	//WideCharToMultiByte(CP_GBK)
//}


/*
__forceinline CString GetFileVersion()
{
int iVerInfoSize;
char *pBuf;
CString asVer= _T("");
VS_FIXEDFILEINFO *pVsInfo;
unsigned int iFileInfoSize = sizeof(VS_FIXEDFILEINFO);
TCHAR fileName[MAX_PATH];
GetModuleFileName(NULL, fileName, MAX_PATH);
iVerInfoSize = GetFileVersionInfoSize(fileName, NULL);

if(iVerInfoSize != 0){
pBuf = new char[iVerInfoSize];
if(GetFileVersionInfo(fileName, 0, iVerInfoSize, pBuf)){
if(VerQueryValue(pBuf, _T("//"), (void**)&pVsInfo, &iFileInfoSize)){
asVer.Format(_T("%d.%d.%d.%d"),
HIWORD(pVsInfo->dwFileVersionMS),
LOWORD(pVsInfo->dwFileVersionMS),
HIWORD(pVsInfo->dwFileVersionLS),
LOWORD(pVsInfo->dwFileVersionLS));
}
}
delete pBuf;
}
return asVer;
}
*/
