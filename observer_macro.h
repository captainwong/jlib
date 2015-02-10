
// place this macro in your class's header file, in your class's definition
#define DECLARE_OBSERVER(callback, param_type) \
protected: \
	typedef struct callback##Info { \
		DECLARE_UNCOPYABLE(callback##Info) \
	public: \
		callback##Info() : _udata(NULL), _on_result(NULL) {} \
		callback##Info(void* udata, callback on_result) : _udata(udata), _on_result(on_result) {} \
		void* _udata; \
		callback _on_result; \
	}callback##Info; \
	typedef callback##Info _callbackInfo; \
	typedef callback _callback; \
	typedef param_type _param_type; \
	std::list<_callbackInfo *> _observerList; \
	CLock _lock4ObserverList; \
public: \
	void RegisterObserver(void* udata, callback cb); \
	void UnRegisterObserver(void* udata); \
	void NotifyObservers(_param_type param);
	

// place this macro in your class's cpp file
#define IMPLEMENT_OBSERVER(class_name) \
void class_name::RegisterObserver(void* udata, _callback cb) \
{ \
	_lock4ObserverList.Lock(); \
	std::list<_callbackInfo *>::iterator iter = _observerList.begin(); \
	while (iter != _observerList.end()) { \
		_callbackInfo* observer = *iter; \
		if (observer->_udata == udata) { \
			_lock4ObserverList.UnLock(); \
			return; \
		} \
		iter++; \
	} \
	_callbackInfo *observer = new _callbackInfo(udata, cb); \
	_observerList.insert(iter, observer); \
	_lock4ObserverList.UnLock(); \
} \
void class_name::UnRegisterObserver(void* udata) \
{ \
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



	
	