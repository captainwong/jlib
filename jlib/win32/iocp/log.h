#ifndef ___LOG_H_INCLUDED___
#define ___LOG_H_INCLUDED___

#include "INCLUDE/sync_simple.h"
#include "D:/global/log.h"
///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// A simple class to log messages to the console
// considering multithreading
class Log {
private:
	static QMutex		m_qMutex;
	//static wchar_t				m_buff[1024];
public:
	static void LogMessage(const char* str);
	static void LogMessage(const wchar_t *str);
	static void LogMessage(const wchar_t *str, long num);
	static void LogMessage(const wchar_t *str, long num1, long num2);
	static void LogMessage(const wchar_t *str, long num, const wchar_t *str1);
	static void LogMessage(const wchar_t *str, const wchar_t *str1);
	static void LogMessage(const wchar_t *str, const wchar_t *str1, const wchar_t *str2);
};

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- IMPLEMENTATION -------------------------------------------------

QMutex Log::m_qMutex;

void Log::LogMessage(const wchar_t *str, long num1, long num2) {
	m_qMutex.Lock();
	//wsprintf(m_buff, str, num1, num2);
	CLog::WriteLogW(str, num1, num2);
	m_qMutex.Unlock();
}

void Log::LogMessage(const wchar_t *str, long num, const wchar_t *str1) {
	m_qMutex.Lock();
	CLog::WriteLogW(str, num, str1);
	m_qMutex.Unlock();
}

void Log::LogMessage(const wchar_t *str, const wchar_t *str1, const wchar_t *str2) {
	m_qMutex.Lock();
	CLog::WriteLogW(str, str1, str2);
	m_qMutex.Unlock();
}

void Log::LogMessage(const wchar_t *str, const wchar_t *str1) {
	m_qMutex.Lock();
	CLog::WriteLogW(str, str1);
	m_qMutex.Unlock();
}

void Log::LogMessage(const wchar_t *str, long num) {
	m_qMutex.Lock();
	CLog::WriteLogW(str, num);
	m_qMutex.Unlock();
}

void Log::LogMessage(const wchar_t *str) {
	m_qMutex.Lock();
	CLog::WriteLogW(str);
	m_qMutex.Unlock();
}

void Log::LogMessage(const char *str) {
	m_qMutex.Lock();
	CLog::WriteLogA(str);
	m_qMutex.Unlock();
}
#endif