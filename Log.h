// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////
/*
日志类CLog
简述：单例的日志输出类，使用时会自动构造，但因为要保证日志类最后一个析构，需要手动释放
功能：写日志，输出TRACE内容到dbgview,console。
使用：
	使用函数
		static void SetOutputLogFileName(LPCTSTR szFileName);
		static void SetOutputConsole(BOOL bFlag);
		static void SetOutputDbgView(BOOL bFlag);
		static void SetOutputLogFile(BOOL bFlag);
		来设置输出日志的方式
	使用TRACE或使用函数static void WriteLog(const char* format, ...);写日志，可接受可变参数
释放：在程序退出时（ExitInstance()函数的最后）调用CLog::Release()释放引用计数
*/
#if !defined(AFX_ERRORLOG_H__46D664F1_E737_46B7_9813_2EF1415FF884__INCLUDED_)
#define AFX_ERRORLOG_H__46D664F1_E737_46B7_9813_2EF1415FF884__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <assert.h>

//#include "dbghlpapi.h"

//#ifdef TRACE
//#undef TRACE
//#define TRACE CLog::WriteLog
//#endif

#define IMPLEMENT_CLASS_LOG_STATIC_MEMBER \
	CRITICAL_SECTION CLog::m_cs;\
	CLock CLog::m_lockForSingleton;\
	char CLog::m_ansiBuf[MAX_OUTPUT_LEN];\
	wchar_t CLog::m_utf16Buf[MAX_OUTPUT_LEN];\
	CLog* CLog::m_pInstance = NULL;

/*
#define TRACEFUNCNAME \
	CString str = _T("");\
	INIT_SYM(TRUE);\
	str.Format("File %s, line %d, funcname %s\n", __FILE__, __LINE__, __FUNCNAME__);\
	OutputDebugString(str);\
	UNINIT_SYM();\
*/
#define MAX_OUTPUT_LEN 4096
#define MAX_FILE_LEN 1024 * 1024 * 10
static TCHAR g_szFileName[1024] = {0};

class CLog
{
private:	
	double exe_time; 
	LARGE_INTEGER freq; 
	LARGE_INTEGER start_t, stop_t; 
	BOOL m_bOutputLogFile;
	BOOL m_bOutputDbgView;
	BOOL m_bOutputConsole;
	BOOL m_bConsoleOpened;
	BOOL m_bDbgviewOpened;
	BOOL m_bLogFileOpened;
	BOOL m_bRunning;
	FILE *m_pLogFile;
	PROCESS_INFORMATION pi;
	static CRITICAL_SECTION m_cs;
	static CLock m_lockForSingleton;
	static char m_ansiBuf[MAX_OUTPUT_LEN];
	static wchar_t m_utf16Buf[MAX_OUTPUT_LEN];
	static CLog* m_pInstance;
public:
	static const wchar_t* GetPrivateLogPath(){
		static wchar_t strPrivateMapFolder[1024] = { 0 };
		static BOOL b = TRUE;
		if(b){
			wsprintf(strPrivateMapFolder, _T("%s\\Log"), GetModuleFilePath());
			b = FALSE;
		}
		return strPrivateMapFolder;
	}

	static CLog* GetInstance(){
		m_lockForSingleton.Lock();
		if (CLog::m_pInstance == NULL) {
			static CLog log;
			CLog::m_pInstance = &log;
		}
		m_lockForSingleton.UnLock();
		return CLog::m_pInstance;
	}

	~CLog(){
		m_bRunning = FALSE;
		//WaitTillThreadExited(m_handle);
		if(m_bConsoleOpened)
			OpenConsole(FALSE);
		if(m_bDbgviewOpened)
			OpenDbgview(FALSE);
		if(m_bLogFileOpened)
			CloseLogFile();	
		DeleteCriticalSection(&m_cs);
#ifdef _DEBUG
		TCHAR buf[1024], out[1024];
		wsprintf(buf, _T("%s\n"), _T("CLog::~CLog() destruction"));
		FormatBuf(buf, out, 1024);
		OutputDebugString(out);
#endif
	}

	static void SetOutputLogFileName(LPCTSTR szLastName){
		lstrcpy(g_szFileName, szLastName);
	}

	static void SetOutputConsole(BOOL bFlag){
		CLog* plog = CLog::GetInstance();
		plog->m_bOutputConsole = bFlag;
		plog->OpenConsole(bFlag);
	}

	static void SetOutputDbgView(BOOL bFlag){
		CLog* plog = CLog::GetInstance();
		plog->m_bOutputDbgView = bFlag;
		//plog->OpenDbgview(bFlag);
	}

	static void SetOutputLogFile(BOOL bFlag){
		CLog* plog = CLog::GetInstance();
		plog->m_bOutputLogFile = bFlag;
		if(plog->m_bOutputLogFile && lstrlen(g_szFileName) == 0)
			plog->CreateFileName();
	}

	static void WriteLog(const TCHAR* format, ...)	{
		try{
			CLog* plog = CLog::GetInstance();
			if(plog == NULL)
				return;
			CLocalLock lock(&m_cs);
			if(plog->m_bOutputLogFile || plog->m_bOutputDbgView || plog->m_bOutputConsole)			{
				static TCHAR buf[MAX_OUTPUT_LEN], output[MAX_OUTPUT_LEN], *p;
				p = buf;
				va_list args;
				va_start(args, format);
				p += _vsntprintf_s(p, sizeof (buf) - 1, sizeof (buf) - 1, format, args);
				va_end(args);
				while ( (p > buf) && _istspace(*(p-1)) )
					*--p = _T('\0');
				*p++ = _T('\r');
				*p++ = _T('\n');
				*p = _T('\0');
				plog->FormatBuf(buf, output);
				plog->Output(output);
			}
		}
		catch(...){
			assert(0);
		}
	}

	static void WriteLogW(const wchar_t* format, ...)	{
		try{
			CLog* plog = CLog::GetInstance();
			if(plog == NULL)
				return;
			CLocalLock lock(&m_cs);
			if(plog->m_bOutputLogFile || plog->m_bOutputDbgView || plog->m_bOutputConsole)			{
				static wchar_t buf[MAX_OUTPUT_LEN], *p;
				p = buf;
				va_list args;
				va_start(args, format);
				p += _vsnwprintf_s(p, sizeof (buf) - 1, sizeof (buf) - 1, format, args);
				va_end(args);
				while ( (p > buf) && iswspace(*(p-1)) )
					*--p = '\0';
				*p++ = '\r';
				*p++ = '\n';
				*p = '\0';
#if defined(_UNICODE) || defined(UNICODE)
				wchar_t output[MAX_OUTPUT_LEN];
				plog->FormatBuf(buf, output);
				plog->Output(output);
#else
				char output[MAX_OUTPUT_LEN];
				if(Utf16ToAnsiUseCharArray(buf, m_ansiBuf, MAX_OUTPUT_LEN)){
					plog->FormatBuf(m_ansiBuf, output);
					plog->Output(output);
				}else{
					char *ansiOut = Utf16ToAnsi(buf);
					plog->FormatBuf(ansiOut, output);
					plog->Output(output);
					SAFEDELETEARR(ansiOut);
				}
#endif
			}
		}
		catch(...){
			assert(0);
		}
	}

	static void WriteLogA(const char* format, ...)	{
		try{
			CLog* plog = CLog::GetInstance();
			if(plog == NULL)
				return;
			CLocalLock lock(&m_cs);
			if(plog->m_bOutputLogFile || plog->m_bOutputDbgView || plog->m_bOutputConsole)			{
				static char buf[MAX_OUTPUT_LEN], *p;
				p = buf;
				va_list args;
				va_start(args, format);
				p += _vsnprintf_s(p, sizeof (buf) - 1, sizeof (buf) - 1, format, args);
				va_end(args);
				while ( (p > buf) && isspace(*(p-1)) )
					*--p = '\0';
				*p++ = '\r';
				*p++ = '\n';
				*p = '\0';
#if defined(_UNICODE) || defined(UNICODE)
				wchar_t output[MAX_OUTPUT_LEN];
				if(AnsiToUtf16Array(buf, m_utf16Buf, MAX_OUTPUT_LEN))
				{
					plog->FormatBuf(m_utf16Buf, output);
					plog->Output(output);
				}
				else
				{
					wchar_t *wBuf = AnsiToUtf16(buf);
					plog->FormatBuf(wBuf, output);
					plog->Output(output);
					SAFEDELETEARR(wBuf);
				}			
#else
				char output[MAX_OUTPUT_LEN];
				plog->FormatBuf(buf, output);
				plog->Output(output);
#endif
			}
		}
		catch(...){
			assert(0);
		}
	}

protected:
	CLog(): m_pLogFile(NULL), m_bOutputLogFile(FALSE), m_bOutputDbgView(FALSE), 
		m_bOutputConsole(FALSE), m_bConsoleOpened(FALSE), m_bDbgviewOpened(FALSE), 
		m_bLogFileOpened(FALSE), m_bRunning(TRUE){	
		QueryPerformanceFrequency(&freq); 
		//OutputDebugString(_T("The frequency of your pc is 0x%X.\n"), freq.QuadPart); 
		QueryPerformanceCounter(&start_t); 
		memset(g_szFileName, 0, sizeof(g_szFileName));
		TCHAR out[128] = {0};
		wsprintf(out, _T("CLog construction addr: %p\n"), this);
		OutputDebugString(out);
		memset(&pi, 0, sizeof(pi));
		InitializeCriticalSection(&m_cs);
	}

	LPCTSTR GetAppRunTime()	{
		static TCHAR szTime[128];
		memset(szTime, 0, sizeof szTime);

		QueryPerformanceCounter(&stop_t);
		exe_time = 1e3*(stop_t.QuadPart-start_t.QuadPart)/freq.QuadPart;
		WORD day, hour, min, sec;

		sec		= static_cast<unsigned short>(static_cast<unsigned long>(exe_time / 1000) % 60);
		min		= static_cast<unsigned short>(static_cast<unsigned long>(exe_time / 1000 / 60) % 60);
		hour	= static_cast<unsigned short>((exe_time / 1000 / 60) / 60);
		day		= static_cast<unsigned short>(exe_time / 1000.0 / 60.0 / 60.0 / 24);

		double ms = exe_time - (int)(exe_time) + ((int)(exe_time) % 1000);
		wsprintf(szTime, _T("%dday%02d:%02d:%02ds.%3.6fms"), day, hour, min, sec, ms);
		return szTime;
	}

	void Output(const TCHAR *out)	{
		if(m_bOutputLogFile){
			OutputFile(out);
		}
#ifdef _DEBUG
		OutputDebugString(out);
#else
		if(m_bOutputDbgView){
			OutputDebugString(out);
		}
#endif
		if(m_bOutputConsole){
			_tprintf(out);
		}
	}

	WORD FormatBuf(const TCHAR *oldBuf, TCHAR *newBuf, WORD max_new_buff_len = MAX_OUTPUT_LEN){
		static SYSTEMTIME st;
		static TCHAR sztime[128];
		memset(sztime, 0, sizeof sztime);
		GetLocalTime(&st);
		wsprintf(sztime, _T("HB %04d-%02d-%02d-w%d %02d:%02d:%02d.%03d ---- "), 
			st.wYear, st.wMonth, st.wDay, (st.wDayOfWeek != 0) ? st.wDayOfWeek : 7, 
			st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		lstrcpy(newBuf, sztime);
		if(lstrlen(sztime) + lstrlen(oldBuf) < max_new_buff_len){
			lstrcat(newBuf, oldBuf);
		}
		return static_cast<unsigned short>(lstrlen(newBuf));
	}

	void OutputFile(const TCHAR *buf){
		if(m_pLogFile == NULL)
			m_pLogFile = OpenLogFile();
		if(m_pLogFile != NULL){
			fseek(m_pLogFile, 0, SEEK_END);
			long filelen = ftell(m_pLogFile);
			if(filelen >= MAX_FILE_LEN){
				fclose(m_pLogFile);
				CreateFileName();
				m_pLogFile = OpenLogFile();
			}
		}
		if(m_pLogFile != NULL){
			fseek(m_pLogFile, 0, SEEK_END);
#if defined(_UNICODE) || defined(UNICODE)
			if(Utf16ToAnsiUseCharArray(buf, m_ansiBuf, MAX_OUTPUT_LEN)){
				fwrite(m_ansiBuf, 1, strlen(m_ansiBuf), m_pLogFile);
			}else{
				char *ansiOut = Utf16ToAnsi(buf);
				fwrite(ansiOut, 1, strlen(ansiOut), m_pLogFile);
				SAFEDELETEARR(ansiOut);
			}
#else
			fwrite(buf, 1, strlen(buf), m_pLogFile);
#endif
			fflush(m_pLogFile);
		}
	}

	BOOL OpenDbgview(BOOL bOpen){
		static STARTUPINFO si;
		static LPCTSTR lpszAppName = _T("dbgview.exe");
		if(bOpen){
			if(!m_bDbgviewOpened){
				memset(&si,0,sizeof(STARTUPINFO));
				si.cb = sizeof(STARTUPINFO);
				si.dwFlags = STARTF_USESHOWWINDOW;
				si.wShowWindow = SW_SHOW;
				if(CreateProcess(lpszAppName,NULL,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi))				{
					m_bDbgviewOpened = TRUE;
				}
			}
		}else if(m_bDbgviewOpened){
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			m_bDbgviewOpened = FALSE;
		}

		return bOpen ? m_bDbgviewOpened : !m_bDbgviewOpened;
	}

	FILE* OpenLogFile(){
		FILE *pfile = NULL;
		_tfopen_s(&pfile, g_szFileName, _T("r"));
		if(pfile == NULL){
			_tfopen_s(&pfile, g_szFileName, _T("wb"));
			if(pfile == NULL){
				MessageBox(NULL, _T("Create log file failed."), NULL, 0);
				ASSERT(0);
				return NULL;
			}
		}else{
			fclose(pfile);
			_tfopen_s(&pfile, g_szFileName, _T("ab"));
			if(pfile == NULL){
				MessageBox(NULL, _T("Open log file failed."), NULL, 0);
				ASSERT(0);
				return NULL;
			}
		}
		if(pfile != NULL)
			m_bLogFileOpened = TRUE;
		return pfile;
	}

	void CreateFileName(){
		SYSTEMTIME st;
		GetLocalTime(&st);
		const wchar_t* path = GetPrivateLogPath();
		CreateDirectory(path, NULL);
		wchar_t exe[1024] = { 0 };
		GetModuleFileName(NULL, exe, 1023);
		wsprintf(g_szFileName, _T("%s\\%s-%04d-%02d-%02d-w%d-%02d-%02d-%02d"), 
				 path, CFileOper::GetFileNameFromPathName(exe),
				 st.wYear, st.wMonth, st.wDay,
				 (st.wDayOfWeek != 0) ? st.wDayOfWeek : 7,
				 st.wHour, st.wMinute, st.wSecond);
		lstrcat(g_szFileName, _T(".log"));
	}

	void CloseLogFile(){
		if(m_pLogFile != NULL){
			fflush(m_pLogFile);
			fclose(m_pLogFile);
			m_pLogFile = NULL;
		}
	}

	BOOL OpenConsole(BOOL bOpen){
		static FILE *pConsole = NULL;
		if(bOpen){
			if(!m_bConsoleOpened){
				if(AllocConsole()){
					SetConsoleTitle(_T("output"));
					/*COORD coord;
					coord.X = 120;
					coord.Y = 60;
					HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
					FillConsoleOutputAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
						120, coord, NULL);
					SetConsoleScreenBufferSize(hStdOut, coord);
					SMALL_RECT se;
					se.Left = 0;
					se.Top = 0;
					se.Right = 100;
					se.Bottom = 100;
					SetConsoleWindowInfo(hStdOut, FALSE, &se);
					*/
					_tfreopen_s(&pConsole, _T("CONOUT$"), _T("w"), stdout);
					m_bConsoleOpened = TRUE;
				}
			}
			return m_bConsoleOpened;
		}else{
			if(m_bConsoleOpened){
				fclose(pConsole);
				if(FreeConsole()){				
					m_bConsoleOpened = FALSE;
				}
			}
			return !m_bConsoleOpened;
		}
	}

	LPCTSTR GetLastTimeLogFileName(){
		return g_szFileName;
	}
};

//CRITICAL_SECTION CLog::m_cs;
//int CLog::m_nRef = 0;
//char CLog::m_ansiBuf[MAX_OUTPUT_LEN] = {0};
//CLock CLog::m_lockForSingleton;

#endif // !defined(AFX_ERRORLOG_H__46D664F1_E737_46B7_9813_2EF1415FF884__INCLUDED_)
