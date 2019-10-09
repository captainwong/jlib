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

#ifdef _DEBUG
#define VERIFYPTR(p, cb) {MTVERIFY(!IsBadWritePtr(p, cb));}
#else
#define VERIFYPTR(p, cb)
#endif

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
