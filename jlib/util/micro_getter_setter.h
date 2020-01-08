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

#define DECLARE_GETTER_WCHAR_CSTR(val) \
	const wchar_t* get##val() const { \
		return val; \
	}

#define DECLARE_SETTER_WCHAR_CSTR(val) \
	void set##val(const wchar_t* param) { \
		if (param) { \
			int len = wcslen(param); \
			if (val) { delete[] val; } \
			val = new wchar_t[len + 1]; \
			wcscpy_s(val, len + 1, param); \
		} else { \
			if (val) { delete[] val; } \
			val = new wchar_t[1]; \
			val[0] = '\0'; \
		} \
	}

#define DECLARE_GETTER_SETTER_WCHAR_STR(val) \
	DECLARE_SETTER_WCHAR_CSTR(val); \
	DECLARE_SETTER_WCHAR_CSTR(val);


#define INITIALIZE_STRING(val) { val = new wchar_t[1]; val[0] = '\0'; }

//! public: int member;
#define DECLREA_MEMBER(region, type, val) \
region: \
	type val;

//! public: int member = {};
#define DECLREA_MEMBER_DEFAULT_INITED(region, type, val) \
region: \
	type val = {};

//! public: int getVal() const { return val; }
#define DECLARE_GETTER_WITH_REGION_AND_NAME(region, type, val, getter) \
region: \
	type getter() const { return val; }

//! private: int val; public: int getVal() const { return val; }
#define DECLARE_MEMBER_WITH_GETTER(mregion, type, val, fregion, getter) \
DECLREA_MEMBER(mregion, type, val) \
DECLARE_GETTER_WITH_REGION_AND_NAME(fregion, type, val, getter)

//! private: int val = {}; public: int getVal() const { return val; }
#define DECLARE_MEMBER_DEFAULT_INITED_WITH_GETTER(mregion, type, val, fregion, getter) \
DECLREA_MEMBER_DEFAULT_INITED(mregion, type, val) \
DECLARE_GETTER_WITH_REGION_AND_NAME(fregion, type, val, getter)


#define DECLARE_PRI_M_PUB_G(type, val, getter) \
DECLREA_MEMBER(private, type, val) \
DECLARE_GETTER_WITH_REGION_AND_NAME(public, type, val, getter)

#define DECLARE_PRI_MDI_PUB_G(type, val, getter) \
DECLREA_MEMBER_DEFAULT_INITED(private, type, val) \
DECLARE_GETTER_WITH_REGION_AND_NAME(public, type, val, getter)
