#pragma once

namespace jlib {
// place this macro in your class's header file, in your class's definition
#define DECLARE_OBSERVER(callback, param_type) \
protected: \
	typedef callback _callback_type; \
	typedef const param_type _param_type; \
	typedef struct callback##Info { \
		DECLARE_UNCOPYABLE(callback##Info) \
	public: \
		callback##Info() : _udata(NULL), _on_result(NULL) {} \
		callback##Info(void* udata, _callback_type on_result) : _udata(udata), _on_result(on_result) {} \
		void* _udata; \
		_callback_type _on_result; \
	}_callbackInfo; \
	std::list<_callbackInfo *> _observerList; \
	CLock _lock4ObserverList; \
public: \
	void RegisterObserver(void* udata, _callback_type cb); \
	void UnRegisterObserver(void* udata); \
	void NotifyObservers(_param_type param);
	

// place this macro in your class's cpp file
#define IMPLEMENT_OBSERVER(class_name) \
void class_name::RegisterObserver(void* udata, _callback_type cb) \
{ \
	AUTO_LOG_FUNCTION; \
	_lock4ObserverList.Lock(); \
	_callbackInfo *observer = new _callbackInfo(udata, cb); \
	_observerList.push_back(observer); \
	_lock4ObserverList.UnLock(); \
} \
void class_name::UnRegisterObserver(void* udata) \
{ \
	AUTO_LOG_FUNCTION; \
	_lock4ObserverList.Lock(); \
	std::list<_callbackInfo *>::iterator iter = _observerList.begin(); \
	while (iter != _observerList.end()) { \
		_callbackInfo* observer = *iter; \
		if (observer->_udata == udata) { \
			delete observer; \
			_observerList.erase(iter); \
			break; \
		} \
		iter++; \
	} \
	_lock4ObserverList.UnLock(); \
} \
void class_name::NotifyObservers(_param_type param) \
{ \
	AUTO_LOG_FUNCTION; \
	_lock4ObserverList.Lock(); \
	std::list<_callbackInfo *>::iterator iter = _observerList.begin(); \
	while (iter != _observerList.end()) { \
		_callbackInfo * observer = *iter++; \
		observer->_on_result(observer->_udata, param); \
	} \
	_lock4ObserverList.UnLock(); \
}


// place this macro in your class's destruct function.
#define DESTROY_OBSERVER \
{ \
	std::list<_callbackInfo *>::iterator iter = _observerList.begin(); \
	while (iter != _observerList.end()) { \
		_callbackInfo * observer = *iter++; \
		delete observer; \
	} \
	_observerList.clear(); \
}

};

	
	