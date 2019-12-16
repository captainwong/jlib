#pragma once


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

#define DECLARE_GETTER_SETTER_CSTRING(val) \
	DECLARE_GETTER(CString, val); \
	DECLARE_SETTER(CString, val);


#define INITIALIZE_STRING(val) { val = new wchar_t[1]; val[0] = 0; }

