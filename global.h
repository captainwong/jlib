#pragma once



#include "C:/Global/LocalLock.h"
#include "C:/Global/mtverify.h"
#include "C:/Global/FileOper.h"
#include "C:/Global/Log.h"
#define LOG CLog::WriteLog
#define LOGA CLog::WriteLogA
#define LOGW CLog::WriteLogW
#define LOGB(b, l) CLog::Dump(b, l)

class LogFunction {
private:
	const char* _func_name;
public:
	LogFunction(const char* func_name) : _func_name(func_name) { LOGA("%s in\n", _func_name); }
	~LogFunction() { LOGA("%s out\n", _func_name); }
};

#define LOG_FUNCTION(func_name) LogFunction _log_function_object(func_name);
#define AUTO_LOG_FUNCTION LOG_FUNCTION(__FUNCTION__)

#include "C:/Global/MyWSAError.h"
#include "c:/Global/observer_macro.h"

#define NAMESPACE_END };

#define DECLARE_UNCOPYABLE(classname) \
private:\
	classname(const classname&) {}\
	classname& operator=(const classname&) {}

// singleton
//class CSingletonClassReleaseHelper {
//public:
//	CSingletonClassReleaseHelper() {}
//	~CSingletonClassReleaseHelper() {}
//private:
//
//};

#define DECLARE_SINGLETON(class_name) \
private: \
	class_name(); \
	static class_name* m_pInstance; \
	static CLock m_lock4Instance; \
public: \
	static class_name* GetInstance() { \
		m_lock4Instance.Lock(); \
		if (m_pInstance == NULL){ \
			m_pInstance = new class_name(); \
		} \
		m_lock4Instance.UnLock(); \
		return m_pInstance; \
	} \
	static void ReleaseObject() { \
		if (m_pInstance) { delete m_pInstance; m_pInstance = NULL; } \
	}


#define IMPLEMENT_SINGLETON(class_name) \
	class_name* class_name::m_pInstance = NULL; \
	CLock class_name::m_lock4Instance;

// getter & setter
#define DECLARE_GETTER(type, val) \
	type get##val() const { \
		return val; \
	}

#define DECLARE_SETTER(type, val) \
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
	DECLARE_GETTER_STRING(val); \
	DECLARE_SETTER_STRING(val);


#define INITIALIZE_STRING(val) { val = new wchar_t[1]; val[0] = 0; }
