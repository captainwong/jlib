#pragma once

/**
* 主要参考了 https://blog.csdn.net/breaksoftware/article/category/9269057
*/

#include <Windows.h>
#include <WbemIdl.h>
#include <atlbase.h>
#include <atlcom.h>
#include <comdef.h>
#include <string>
#include <functional>
#include <assert.h>
#include <vector>
#include <unordered_map>
#include "../util/str_util.h"

#pragma comment (lib, "comsuppw.lib")
#pragma comment (lib, "wbemuuid.lib")

#if !defined(UNICODE) && !defined(_UNICODE)
#error jlib::win32::wmi only works with unicode!
#endif

#define OUTPUT_ERROR_AND_BREAK(hr, errorFunc) if (errorFunc) { \
	std::wstring msg = _com_error(hr).ErrorMessage(); \
	msg += L" "; msg += __FILEW__; msg += L":"; msg += std::to_wstring(__LINE__); \
	errorFunc(hr, msg); \
} \
break;

#define JLIB_CHECK_HR(hr) if (FAILED(hr)) break;
#define JLIB_CHECK_HR2(hr, errorFunc) if (FAILED(hr)) { OUTPUT_ERROR_AND_BREAK(hr, errorFunc); }

#define JLIB_CHECK_WMI_HR(hr) if ((hr) != WBEM_S_NO_ERROR) break;
#define JLIB_CHECK_WMI_HR2(hr, errorFunc) if ((hr) != WBEM_S_NO_ERROR) { OUTPUT_ERROR_AND_BREAK(hr, errorFunc); }

namespace jlib
{
namespace win32
{
namespace wmi
{

typedef std::function<void(HRESULT, const std::wstring&)> ErrorFunc;
typedef std::function<void(const std::wstring&, const std::wstring&)> OutputFunc;

//struct ResultItem {
//	std::wstring key = {};
//	std::wstring value = {};
//};

typedef std::unordered_map<std::wstring, std::wstring> ResultItem;
typedef std::vector<ResultItem> Result;

class WmiBase
{
public:
	WmiBase(const std::wstring& _namespace, OutputFunc outputFunc, ErrorFunc errorFunc = nullptr)
		: namespace_(_namespace)
		, outputFunc_(outputFunc)
		, errorFunc_(errorFunc)
	{

	}

	~WmiBase() {
		services_.Release();
		CoUninitialize();
	}

	/*
	* @brief 快速查询
	* @note 默认namespace 为 root\\CIMV2
	* @param keys 要查询的键名，例如 Caption, Desc, ..., 必须有序，否则结果的key:value无法对应
	* @param provider 例如 Win32_Process 
	* @param[in|out] result 结果
	* @return 成功或失败
	* @note 例：keys=["captian, key"], provider="Win32_Process", 则result结构应为：[ {caption: value, key:value}, {caption: value, key:value}, ...]
	*/
	static bool simpleSelect(const std::vector<std::wstring>& keys, const std::wstring& provider, Result& result) {
		std::vector<std::wstring> values, errors;
		auto output = [&values](const std::wstring& key, const std::wstring& value) {
			values.push_back(value);
		};

		auto error = [&errors](HRESULT hr, const std::wstring& msg) {
			errors.push_back(msg);
		};

		wmi::WmiBase wmi(L"ROOT\\CIMV2", output, error);
		if (!wmi.prepare()) {
			return false;
		}

		if (!wmi.execute(std::wstring(L"select ") + join(keys, std::wstring(L",")) + L" from " + provider) || !errors.empty()) {
			return false;
		}

		for (size_t i = 0; i < values.size(); i += keys.size()) {
			ResultItem item;
			for (size_t j = 0; j < keys.size(); j++) {
				item.insert({ keys.at(j % keys.size()), values.at(i + j) });				
			}
			result.emplace_back(item);
		}

		return true;
	}

	bool prepare() {
		do {
			HRESULT hr = E_FAIL;
			if (!initCom(hr)) {
				if (errorFunc_) {
					errorFunc_(hr, _com_error(hr).ErrorMessage());
				}
				break; 
			}

			hr = setComSecurityLevel();
			JLIB_CHECK_HR2(hr, errorFunc_);

			CComPtr<IWbemLocator> locator = {};
			hr = obtainWbemLocator(locator);
			JLIB_CHECK_HR2(hr, errorFunc_);

			hr = connect(locator, services_);
			JLIB_CHECK_HR2(hr, errorFunc_);

			hr = setProxySecurityLevel(services_);
			JLIB_CHECK_HR2(hr, errorFunc_);

			return true;
		} while (0);

		return false;
	}


	bool execute(const std::wstring& wql) {
		HRESULT hr = WBEM_S_FALSE;

		do {
			assert(services_);
			CComPtr<IEnumWbemClassObject> pEnumerator = NULL;
			hr = services_->ExecQuery(
				CComBSTR("WQL"),
				CComBSTR(wql.c_str()),
				WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
				NULL,
				&pEnumerator);

			JLIB_CHECK_WMI_HR2(hr, errorFunc_);

			ULONG uReturn = 0;

			while (pEnumerator) {
				CComPtr<IWbemClassObject> object = NULL;
				hr = pEnumerator->Next(WBEM_INFINITE, 1, &object, &uReturn);
				if (0 == uReturn) { break; }
				JLIB_CHECK_WMI_HR2(hr, errorFunc_);
				parseIWbemClassObject(object);
			}

		} while (0);

		return !FAILED(hr);
	}

protected:
	bool initCom(HRESULT& hr) {
		hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (FAILED(hr)) {
			if (hr == 0x80010106) {
				// already initilized, pass
			} else {
				hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
				if (FAILED(hr)) {
					return false;
				}
			}
		}
		return true;
	}

	HRESULT setComSecurityLevel() {
		return CoInitializeSecurity(
			NULL,
			-1,
			NULL,
			NULL,
			RPC_C_AUTHN_LEVEL_DEFAULT,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			EOAC_NONE,
			NULL
		);
	}

	HRESULT obtainWbemLocator(CComPtr<IWbemLocator>& locator) {
		return CoCreateInstance(
			CLSID_WbemLocator,
			0,
			CLSCTX_INPROC_SERVER,
			IID_IWbemLocator,
			(LPVOID*)& locator
		);
	}

	HRESULT connect(CComPtr<IWbemLocator> locator, CComPtr<IWbemServices>& services) {
		return locator->ConnectServer(CComBSTR(namespace_.data()),
									  NULL, NULL, NULL, 0, NULL, NULL, &services);
	}

	HRESULT setProxySecurityLevel(CComPtr<IWbemServices> services) {
		return CoSetProxyBlanket(
			services,					 // Indicates the proxy to set
			RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx 
			RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx 
			NULL,                        // Server principal name 
			RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
			RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
			NULL,                        // client identity
			EOAC_NONE                    // proxy capabilities 
		);
	}

	HRESULT parseIWbemClassObject(CComPtr<IWbemClassObject> object) {
		HRESULT hr = WBEM_S_NO_ERROR;
		do {
			CComVariant vtClass;
			hr = object->BeginEnumeration(WBEM_FLAG_LOCAL_ONLY);

			do {
				CComBSTR key;
				CComVariant value;
				CIMTYPE type;
				LONG flavor = 0;
				hr = object->Next(0, &key, &value, &type, &flavor);
				if (WBEM_S_NO_MORE_DATA == hr) { break; }
				JLIB_CHECK_WMI_HR2(hr, errorFunc_);
				hr = parseSingleItem(key, value, type, flavor);
			} while (WBEM_S_NO_ERROR == hr);

			hr = object->EndEnumeration();
		} while (0);

		return hr;
	}

	HRESULT parseSingleItem(CComBSTR key, CComVariant value, CIMTYPE type, LONG flavor) {
		HRESULT hr = WBEM_S_NO_ERROR;
		switch (value.vt) {
		case VT_UNKNOWN:
			parseUnknownItem(key, value, type, flavor);
			break;
		default:
			parseItem(key, value, type, flavor);
			break;
		}
		return hr;
	}

	void parseItem(CComBSTR key, CComVariant value, CIMTYPE type, LONG flavor) {
		std::wstring Key = key.m_str;
		std::wstring Value;

		switch (value.vt) {
		case VT_BSTR:
			Value = value.bstrVal;
			break;

		case VT_I1:
		case VT_I2:
		case VT_I4:
		case VT_I8:
		case VT_INT:
		case VT_UI8:
		case VT_UI1:
		case VT_UI2:
		case VT_UI4:
		case VT_UINT:
			Value = std::to_wstring(value.intVal);
			break;

		case VT_BOOL:
			Value = value.boolVal ? L"TRUE" : L"FALSE";
			break;

		case VT_NULL:
			Value = L"NULL";
			break;

		default:
			//if(value.vt)
			//ATLASSERT(FALSE);
			Value = L"Failed with vt=" + std::to_wstring(value.vt);
			break;
		}

		outputFunc_(Key, Value);
	}

	HRESULT parseUnknownItem(CComBSTR key, CComVariant value, CIMTYPE type, LONG flavor) {
		HRESULT hr = WBEM_S_NO_ERROR;
		if (NULL == value.punkVal) {
			return hr;
		}
		// object类型转换成IWbemClassObject接口指针，通过该指针枚举其他属性
		CComPtr<IWbemClassObject> pObjInstance = (IWbemClassObject*)value.punkVal;

		do {
			hr = pObjInstance->BeginEnumeration(WBEM_FLAG_LOCAL_ONLY);
			JLIB_CHECK_HR2(hr, errorFunc_);
			CComBSTR newKey;
			CComVariant newValue;
			CIMTYPE newtype;
			LONG newFlavor = 0;
			hr = pObjInstance->Next(0, &newKey, &newValue, &newtype, &newFlavor);
			JLIB_CHECK_HR2(hr, errorFunc_);
			parseItem(newKey, newValue, newtype, newFlavor);
		} while (WBEM_S_NO_ERROR == hr);

		hr = pObjInstance->EndEnumeration();
		return WBEM_S_NO_ERROR;
	}

protected:
	std::wstring namespace_ = {};
	CComPtr<IWbemServices> services_ = {};
	OutputFunc outputFunc_ = {};
	ErrorFunc errorFunc_ = {};

};

} // namespace wmi
} // namespace win32
} // namespace jlib
