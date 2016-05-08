#pragma once

#include <string>
#include <chrono>
#include <mutex>
#include "utf8.h"
//#include "LocalLock.h"
#include "mtverify.h"
#include "FileOper.h"
#include "Log.h"
#include "MyWSAError.h"
#include "observer_macro.h"

namespace jlib {
#define JLOG CLog::WriteLogW
#define JLOGA CLog::WriteLogA
#define JLOGW CLog::WriteLogW
#define JLOGB(b, l) CLog::Dump(b, l)
#define JLOGASC(b, l) CLog::DumpAsc(b, l)

class LogFunction {
private:
	const char* _func_name;
	std::chrono::system_clock::time_point _begin;
public:
	LogFunction(const char* func_name) : _func_name(func_name) {
		JLOGA("%s in\n", _func_name); _begin = std::chrono::system_clock::now();
	}
	~LogFunction() { 
		auto diff = std::chrono::system_clock::now() - _begin;
		auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
		JLOGA("%s out, duration: %d(ms)\n", _func_name, msec.count()); 
	}
};

#define LOG_FUNCTION(func_name) LogFunction _log_function_object(func_name);
#define AUTO_LOG_FUNCTION LOG_FUNCTION(__FUNCTION__)

class range_log
{
private:
	std::wstring _msg;
	std::chrono::system_clock::time_point _begin;
public:
	range_log(const std::wstring& msg) : _msg(msg) { JLOG((_msg + L" in").c_str()); _begin = std::chrono::system_clock::now(); }
	~range_log() {
		auto diff = std::chrono::system_clock::now() - _begin;
		auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
		JLOG(L"%s out, duration: %d(ms)\n", _msg.c_str(), msec.count());
	}
};

#define NAMESPACE_END };

#define DECLARE_UNCOPYABLE(classname) \
	classname(const classname&) = delete; \
	classname& operator=(const classname&) = delete;


#define DECLARE_SINGLETON(class_name) \
private: \
	class_name(); \
	static class_name* m_pInstance; \
	static std::mutex m_lock4Instance; \
public: \
	static class_name* GetInstance() { \
		std::lock_guard<std::mutex> lock(m_lock4Instance); \
		if (m_pInstance == NULL){ \
			m_pInstance = new class_name(); \
		} \
		return m_pInstance; \
	} \
	static void ReleaseObject() { \
		if (m_pInstance) { delete m_pInstance; m_pInstance = NULL; } \
	}


#define IMPLEMENT_SINGLETON(class_name) \
	class_name* class_name::m_pInstance = NULL; \
	std::mutex class_name::m_lock4Instance;

// getter & setter
#define DECLARE_GETTER(type, val) \
	type get##val() const { \
		return val; \
	}

#define DECLARE_SETTER(type, val) \
	void set##val(const type& param) { \
		val = param;\
	}

#define DECLARE_SETTER_NONE_CONST(type, val) \
	void set##val(type param) { \
		val = param;\
	}

#define DECLARE_GETTER_SETTER(type, val) \
	DECLARE_GETTER(type, val) \
	DECLARE_SETTER(type, val)

#define DECLARE_GETTER_SETTER_INT(val) \
	DECLARE_GETTER(int, val) \
	DECLARE_SETTER(int, val)

#define DECLARE_GETTER_STRING(val) \
	const wchar_t* get##val() const { \
		return val; \
	}

#define DECLARE_SETTER_STRING(val) \
	void set##val(const wchar_t* param) { \
		if (param) { \
			int len = wcslen(param); \
			if (val) { delete[] val; } \
			val = new wchar_t[len + 1]; \
			wcscpy_s(val, len + 1, param); \
		} else { \
			if (val) { delete[] val; } \
			val = new wchar_t[1]; \
			val[0] = 0; \
		} \
	}

#define DECLARE_GETTER_SETTER_STRING(val) \
	DECLARE_GETTER(CString, val); \
	DECLARE_SETTER(CString, val);


#define INITIALIZE_STRING(val) { val = new wchar_t[1]; val[0] = 0; }


inline std::wstring get_exe_path()
{
	wchar_t path[1024] = { 0 };
	GetModuleFileName(nullptr, path, 1024);
	std::wstring::size_type pos = std::wstring(path).find_last_of(L"\\/");
	return std::wstring(path).substr(0, pos);
}


class auto_timer : public boost::noncopyable
{
private:
	int m_timer_id;
	DWORD m_time_out;
	HWND m_hWnd;

public:

	auto_timer(HWND hWnd, int timerId, DWORD timeout) : m_hWnd(hWnd), m_timer_id(timerId), m_time_out(timeout)
	{
		KillTimer(hWnd, m_timer_id);
	}

	~auto_timer()
	{
		SetTimer(m_hWnd, m_timer_id, m_time_out, nullptr);
	}
};

};
