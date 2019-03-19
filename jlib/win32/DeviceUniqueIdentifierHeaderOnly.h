#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <Windows.h>
#include <winioctl.h>
#include <comdef.h>

namespace jlib {
namespace DeviceUniqueIdentifier {


/*
参考1:
《设备唯一标识方法（Unique Identifier）：如何在Windows系统上获取设备的唯一标识》
http://www.vonwei.com/post/UniqueDeviceIDforWindows.html

参考2：
《通过WMI获取网卡MAC地址、硬盘序列号、主板序列号、CPU ID、BIOS序列号》
https://blog.csdn.net/b_h_l/article/details/7764767
*/

enum QueryType : size_t {
	/*
	当前网卡MAC
	MAC地址可能是最常用的标识方法，但是现在这种方法基本不可靠：
	一个电脑可能存在多个网卡，多个MAC地址，
	如典型的笔记本可能存在有线、无线、蓝牙等多个MAC地址，
	随着不同连接方式的改变，每次MAC地址也会改变。
	而且，当安装有虚拟机时，MAC地址会更多。
	MAC地址另外一个更加致命的弱点是，MAC地址很容易手动更改。
	因此，MAC地址基本不推荐用作设备唯一ID。
	*/
	MAC_ADDR = 1,

	/*
	当前网卡原生MAC
	可以轻易修改网卡MAC，但无法修改原生MAC
	*/
	MAC_ADDR_REAL = 1 << 1,

	/*
	硬盘序列号
	在Windows系统中通过命令行运行“wmic diskdrive get serialnumber”可以查看。
	硬盘序列号作为设备唯一ID存在的问题是，很多机器可能存在多块硬盘，
	特别是服务器，而且机器更换硬盘是很可能发生的事情，
	更换硬盘后设备ID也必须随之改变， 不然也会影响授权等应用。
	因此，很多授权软件没有考虑使用硬盘序列号。
	而且，不一定所有的电脑都能获取到硬盘序列号。
	*/
	HARDDISK_SERIAL = 1 << 2,

	//! 主板序列号
	/*
	在Windows系统中通过命令行运行“wmic csproduct get UUID”可以查看。
	主板UUID是很多授权方法和微软官方都比较推崇的方法，
	即便重装系统UUID应该也不会变
	（笔者没有实测重装，不过在一台机器上安装双系统，获取的主板UUID是一样的，
	双系统一个windows一个Linux，Linux下用“dmidecode -s system-uuid”命令可以获取UUID）。
	但是这个方法也有缺陷，因为不是所有的厂商都提供一个UUID，
	当这种情况发生时，wmic会返回“FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF”，
	即一个无效的UUID。
	*/
	MOTHERBOARD_UUID = 1 << 3,

	//! 主板型号
	MOTHERBOARD_MODEL = 1 << 4,

	/*
	CPU ID
	在Windows系统中通过命令行运行“wmic cpu get processorid”就可以查看CPU ID。
	目前CPU ID也无法唯一标识设备，Intel现在可能同一批次的CPU ID都一样，不再提供唯一的ID。
	而且经过实际测试，新购买的同一批次PC的CPU ID很可能一样。
	这样作为设备的唯一标识就会存在问题。
	*/
	CPU_ID = 1 << 5,

	//! BIOS序列号
	BIOS_SERIAL = 1 << 6,

	/*
	Windows 产品ID
	在“控制面板\系统和安全\系统”的最下面就可以看到激活的Windows产品ID信息，
	另外通过注册表“HKEY_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion”也可以看到看到“ProductId”字段。
	不过这个产品ID并不唯一，不同系统或者机器重复的概率也比较大。
	虚拟机中克隆的系统，使用同一个镜像安装激活的系统，其产品ID就可能一模一样。
	经过实测，笔者在两台Thinkpad笔记本上发现其ProductId完全一样。
	*/
	WINDOWS_PRODUCT_ID = 1 << 7,

	/*
	Windows MachineGUID
	Windows安装时会唯一生成一个GUID，
	可以在注册表“HKEY_MACHINE\SOFTWARE\Microsoft\Cryptography”中查看其“MachineGuid”字段。
	这个ID作为Windows系统设备的唯一标识不错，不过值得注意的一点是，与硬件ID不一样，
	这个ID在重装Windows系统后应该不一样了。
	这样授权软件在重装系统后，可能就需要用户重新购买授权。
	*/
	WINDOWS_MACHINE_GUID = 1 << 8,
};

enum {
	RecommendedQueryTypes = WINDOWS_MACHINE_GUID
	| WINDOWS_PRODUCT_ID
	| BIOS_SERIAL
	| CPU_ID
	| MOTHERBOARD_MODEL
	| MOTHERBOARD_UUID
	,

	AllQueryTypes = RecommendedQueryTypes
	| HARDDISK_SERIAL
	| MAC_ADDR_REAL
	| MAC_ADDR
	,
};

/**
* @brief 查询信息
* @param[in] queryTypes QueryType集合
* @param[in,out] results 查询结果集合
* @return 成功或失败
*/
static bool query(size_t queryTypes, std::unordered_map<QueryType, std::wstring>& results);

/**
* @brief 查询信息
* @param[in] queryTypes QueryType集合
* @param[in,out] results 查询结果集合
* @return 成功或失败
*/
static bool query(const std::vector<QueryType>& queryTypes, std::unordered_map<QueryType, std::wstring>& results);

/**
* @brief 连接查询结果为一个字符串
* @param[in] results 查询结果集合
* @param[in,out] conjunction 连词
* @return 将结果以连词连接起来组成的字符串
*/
static std::wstring join_result(const std::vector<std::wstring>& results, const std::wstring& conjunction);

}
}

// Implementation
#include <Windows.h>
#include <algorithm>
#include <math.h>
#include <strsafe.h>
#include <tchar.h>
#include <ntddndis.h>
#include <WbemIdl.h>
#include <comdef.h>
#include <comutil.h>
#include <atlconv.h>
#include <jlib/utf8.h>

#pragma comment (lib, "comsuppw.lib")
#pragma comment (lib, "wbemuuid.lib")

namespace jlib {
namespace DeviceUniqueIdentifier {
namespace detail {

struct DeviceProperty {
	enum { PROPERTY_MAX_LEN = 128 }; // 属性字段最大长度
	wchar_t szProperty[PROPERTY_MAX_LEN];
};

struct WQL_QUERY
{
	const wchar_t* szSelect = nullptr;
	const wchar_t* szProperty = nullptr;
};

inline WQL_QUERY getWQLQuery(QueryType queryType) {
	switch (queryType) {
	case DeviceUniqueIdentifier::MAC_ADDR:
		return // 网卡当前MAC地址
		{ L"SELECT * FROM Win32_NetworkAdapter WHERE (MACAddress IS NOT NULL) AND (NOT (PNPDeviceID LIKE 'ROOT%'))",
			L"MACAddress" };
	case DeviceUniqueIdentifier::MAC_ADDR_REAL:
		return // 网卡原生MAC地址
		{ L"SELECT * FROM Win32_NetworkAdapter WHERE (MACAddress IS NOT NULL) AND (NOT (PNPDeviceID LIKE 'ROOT%'))",
			L"PNPDeviceID" };
	case DeviceUniqueIdentifier::HARDDISK_SERIAL:
		return // 硬盘序列号
		{ L"SELECT * FROM Win32_DiskDrive WHERE (SerialNumber IS NOT NULL) AND (MediaType LIKE 'Fixed hard disk%')",
			L"SerialNumber" };
	case DeviceUniqueIdentifier::MOTHERBOARD_UUID:
		return // 主板序列号
		{ L"SELECT * FROM Win32_BaseBoard WHERE (SerialNumber IS NOT NULL)",
			L"SerialNumber" };
	case DeviceUniqueIdentifier::MOTHERBOARD_MODEL:
		return // 主板型号
		{ L"SELECT * FROM Win32_BaseBoard WHERE (Product IS NOT NULL)",
			L"Product" };
	case DeviceUniqueIdentifier::CPU_ID:
		return // 处理器ID
		{ L"SELECT * FROM Win32_Processor WHERE (ProcessorId IS NOT NULL)",
			L"ProcessorId" };
	case DeviceUniqueIdentifier::BIOS_SERIAL:
		return // BIOS序列号
		{ L"SELECT * FROM Win32_BIOS WHERE (SerialNumber IS NOT NULL)",
			L"SerialNumber" };
	case DeviceUniqueIdentifier::WINDOWS_PRODUCT_ID:
		return // Windows 产品ID
		{ L"SELECT * FROM Win32_OperatingSystem WHERE (SerialNumber IS NOT NULL)",
			L"SerialNumber" };
	default:
		return { L"", L"" };
	}
}


static BOOL WMI_DoWithHarddiskSerialNumber(wchar_t *SerialNumber, UINT uSize)
{
	UINT	iLen;
	UINT	i;

	iLen = wcslen(SerialNumber);
	if (iLen == 40)	// InterfaceType = "IDE"
	{	// 需要将16进制编码串转换为字符串
		wchar_t ch, szBuf[32];
		BYTE b;

		for (i = 0; i < 20; i++) {	// 将16进制字符转换为高4位
			ch = SerialNumber[i * 2];
			if ((ch >= '0') && (ch <= '9')) {
				b = ch - '0';
			} else if ((ch >= 'A') && (ch <= 'F')) {
				b = ch - 'A' + 10;
			} else if ((ch >= 'a') && (ch <= 'f')) {
				b = ch - 'a' + 10;
			} else {	// 非法字符
				break;
			}

			b <<= 4;

			// 将16进制字符转换为低4位
			ch = SerialNumber[i * 2 + 1];
			if ((ch >= '0') && (ch <= '9')) {
				b += ch - '0';
			} else if ((ch >= 'A') && (ch <= 'F')) {
				b += ch - 'A' + 10;
			} else if ((ch >= 'a') && (ch <= 'f')) {
				b += ch - 'a' + 10;
			} else {	// 非法字符
				break;
			}

			szBuf[i] = b;
		}

		if (i == 20) {	// 转换成功
			szBuf[i] = L'\0';
			StringCchCopyW(SerialNumber, uSize, szBuf);
			iLen = wcslen(SerialNumber);
		}
	}

	// 每2个字符互换位置
	for (i = 0; i < iLen; i += 2) {
		std::swap(SerialNumber[i], SerialNumber[i + 1]);
	}

	// 去掉空格
	std::remove(SerialNumber, SerialNumber + wcslen(SerialNumber) + 1, L' ');
	return TRUE;
}


// 通过“PNPDeviceID”获取网卡原生MAC地址
static BOOL WMI_DoWithPNPDeviceID(const wchar_t *PNPDeviceID, wchar_t *MacAddress, UINT uSize)
{
	wchar_t	DevicePath[MAX_PATH];
	HANDLE	hDeviceFile;
	BOOL	isOK = FALSE;

	// 生成设备路径名
	StringCchCopyW(DevicePath, MAX_PATH, L"\\\\.\\");
	StringCchCatW(DevicePath, MAX_PATH, PNPDeviceID);
	// {ad498944-762f-11d0-8dcb-00c04fc3358c} is 'Device Interface Name' for 'Network Card'
	StringCchCatW(DevicePath, MAX_PATH, L"#{ad498944-762f-11d0-8dcb-00c04fc3358c}");

	// 将“PNPDeviceID”中的“/”替换成“#”，以获得真正的设备路径名
	std::replace(DevicePath + 4, DevicePath + 4 + wcslen(PNPDeviceID), (int)L'\\', (int)L'#');

	// 获取设备句柄
	hDeviceFile = CreateFileW(DevicePath,
							  0,
							  FILE_SHARE_READ | FILE_SHARE_WRITE,
							  NULL,
							  OPEN_EXISTING,
							  0,
							  NULL);

	if (hDeviceFile != INVALID_HANDLE_VALUE) {
		ULONG	dwID;
		BYTE	ucData[8];
		DWORD	dwByteRet;

		// 获取网卡原生MAC地址
		dwID = OID_802_3_PERMANENT_ADDRESS;
		isOK = DeviceIoControl(hDeviceFile, IOCTL_NDIS_QUERY_GLOBAL_STATS, &dwID, sizeof(dwID), ucData, sizeof(ucData), &dwByteRet, NULL);
		if (isOK) {	// 将字节数组转换成16进制字符串
			for (DWORD i = 0; i < dwByteRet; i++) {
				StringCchPrintfW(MacAddress + (i << 1), uSize - (i << 1), L"%02X", ucData[i]);
			}

			MacAddress[dwByteRet << 1] = L'\0';	// 写入字符串结束标记
		}

		CloseHandle(hDeviceFile);
	}

	return isOK;
}


static BOOL WMI_DoWithProperty(QueryType queryType, wchar_t *szProperty, UINT uSize)
{
	BOOL isOK = TRUE;
	switch (queryType) {
	case MAC_ADDR_REAL:		// 网卡原生MAC地址		
		isOK = WMI_DoWithPNPDeviceID(szProperty, szProperty, uSize);
		break;
	case HARDDISK_SERIAL:	// 硬盘序列号
		isOK = WMI_DoWithHarddiskSerialNumber(szProperty, uSize);
		break;
	case MAC_ADDR:			// 网卡当前MAC地址
							// 去掉冒号
		std::remove(szProperty, szProperty + wcslen(szProperty) + 1, L':');
		break;
	default:
		// 去掉空格
		std::remove(szProperty, szProperty + wcslen(szProperty) + 1, L' ');
	}
	return isOK;
}



static std::wstring getMachineGUID()
{
	std::wstring res;
	/*using namespace winreg;
	try {
	RegKey key;
	key.Open(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Cryptography");
	res = key.GetStringValue(L"MachineGuid");
	} catch (RegException& e) {
	res = utf8::a2w(e.what());
	} catch (std::exception& e) {
	res = utf8::a2w(e.what());
	} */

	try {

		std::wstring key = L"SOFTWARE\\Microsoft\\Cryptography";
		std::wstring name = L"MachineGuid";

		HKEY hKey;

		if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, key.c_str(), 0, KEY_READ | KEY_WOW64_64KEY, &hKey) != ERROR_SUCCESS)
			throw std::runtime_error("Could not open registry key");

		DWORD type;
		DWORD cbData;

		if (RegQueryValueExW(hKey, name.c_str(), NULL, &type, NULL, &cbData) != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			throw std::runtime_error("Could not read registry value");
		}

		if (type != REG_SZ) {
			RegCloseKey(hKey);
			throw "Incorrect registry value type";
		}

		std::wstring value(cbData / sizeof(wchar_t), L'\0');
		if (RegQueryValueExW(hKey, name.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(&value[0]), &cbData) != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			throw "Could not read registry value";
		}
		if (value.back() == L'\0') {
			value.erase(value.size() - 1, 1);
		}
		res = value;
		RegCloseKey(hKey);

	} catch (std::runtime_error& e) {
		res = utf8::a2w(e.what());
	}
	return res;
}

} // end of namespace detail


static bool query(size_t queryTypes, std::unordered_map<QueryType, std::wstring>& results)
{
	std::vector<QueryType> vec;

	if (queryTypes & MAC_ADDR) {
		vec.push_back(MAC_ADDR);
	}
	if (queryTypes & MAC_ADDR_REAL) {
		vec.push_back(MAC_ADDR_REAL);
	}
	if (queryTypes & HARDDISK_SERIAL) {
		vec.push_back(HARDDISK_SERIAL);
	}
	if (queryTypes & MOTHERBOARD_UUID) {
		vec.push_back(MOTHERBOARD_UUID);
	}
	if (queryTypes & MOTHERBOARD_MODEL) {
		vec.push_back(MOTHERBOARD_MODEL);
	}
	if (queryTypes & CPU_ID) {
		vec.push_back(CPU_ID);
	}
	if (queryTypes & BIOS_SERIAL) {
		vec.push_back(BIOS_SERIAL);
	}
	if (queryTypes & WINDOWS_PRODUCT_ID) {
		vec.push_back(WINDOWS_PRODUCT_ID);
	}

	auto ok = query(vec, results);
	if (queryTypes & WINDOWS_MACHINE_GUID) {
		results[WINDOWS_MACHINE_GUID] = (detail::getMachineGUID());
	}

	return ok;
}

// 基于Windows Management Instrumentation（Windows管理规范）
static bool query(const std::vector<QueryType>& queryTypes, std::unordered_map<QueryType, std::wstring>& results)
{
	bool ok = false;

	// 初始化COM COINIT_APARTMENTTHREADED
	HRESULT hres = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hres)) {
#ifndef __AFXWIN_H__
		_com_error ce(hres);
		qDebug() << "CoInitializeEx with COINIT_MULTITHREADED failed:\n" << ce.Error() << QString::fromWCharArray(ce.ErrorMessage()); // 
#endif

		if (hres == 0x80010106) {
#ifndef __AFXWIN_H__
			qDebug() << "already initilized, pass"; // 
#endif
		} else {
#ifndef __AFXWIN_H__
			qDebug() << "trying CoInitializeEx with COINIT_APARTMENTTHREADED"; // 
#endif
			hres = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

			if (FAILED(hres)) {
#ifndef __AFXWIN_H__
				qDebug() << "CoInitializeEx with COINIT_APARTMENTTHREADED failed, exit";
#endif
				return false;
			}
		}
	}

	// 设置COM的安全认证级别
	hres = CoInitializeSecurity(
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
	if (FAILED(hres)) {
#ifndef __AFXWIN_H__
		qDebug() << "CoInitializeSecurity:" << QString::fromWCharArray(_com_error(hres).ErrorMessage());
#endif
		CoUninitialize();
		return false;
	}

	// 获得WMI连接COM接口
	IWbemLocator *pLoc = NULL;
	hres = CoCreateInstance(
		CLSID_WbemLocator,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		reinterpret_cast<LPVOID*>(&pLoc)
	);
	if (FAILED(hres)) {
#ifndef __AFXWIN_H__
		qDebug() << "CoCreateInstance:" << QString::fromWCharArray(_com_error(hres).ErrorMessage());
#endif
		CoUninitialize();
		return false;
	}

	// 通过连接接口连接WMI的内核对象名"ROOT//CIMV2"
	IWbemServices *pSvc = NULL;
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"),
		NULL,
		NULL,
		NULL,
		0,
		NULL,
		NULL,
		&pSvc
	);
	if (FAILED(hres)) {
#ifndef __AFXWIN_H__
		qDebug() << "ConnectServer:" << QString::fromWCharArray(_com_error(hres).ErrorMessage());
#endif
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	// 设置请求代理的安全级别
	hres = CoSetProxyBlanket(
		pSvc,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE
	);
	if (FAILED(hres)) {
#ifndef __AFXWIN_H__
		qDebug() << "CoSetProxyBlanket:" << QString::fromWCharArray(_com_error(hres).ErrorMessage());
#endif
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	for (auto queryType : queryTypes) {
		if (queryType == WINDOWS_MACHINE_GUID) {
			results[WINDOWS_MACHINE_GUID] = (detail::getMachineGUID());
			continue;
		}

		auto query = detail::getWQLQuery(queryType);

		// 通过请求代理来向WMI发送请求
		IEnumWbemClassObject *pEnumerator = NULL;
		hres = pSvc->ExecQuery(
			bstr_t("WQL"),
			bstr_t(query.szSelect),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL,
			&pEnumerator
		);
		if (FAILED(hres)) {
			//pSvc->Release();
			//pLoc->Release();
			//CoUninitialize();
#ifndef __AFXWIN_H__
			qDebug() << "ExecQuery:\n" << QString::fromWCharArray(query.szSelect) << '\n' << QString::fromWCharArray(_com_error(hres).ErrorMessage());
#endif
			results[queryType] = _com_error(hres).ErrorMessage();
			continue;
		}

		// 循环枚举所有的结果对象  
		while (pEnumerator) {
			IWbemClassObject *pclsObj = NULL;
			ULONG uReturn = 0;

			pEnumerator->Next(
				WBEM_INFINITE,
				1,
				&pclsObj,
				&uReturn
			);

			if (uReturn == 0) {
				break;
			}

			// 获取属性值
			{
				VARIANT vtProperty;
				VariantInit(&vtProperty);
				pclsObj->Get(query.szProperty, 0, &vtProperty, NULL, NULL);
				detail::DeviceProperty deviceProperty{};
				StringCchCopyW(deviceProperty.szProperty, detail::DeviceProperty::PROPERTY_MAX_LEN, vtProperty.bstrVal);
				VariantClear(&vtProperty);
				// 对属性值做进一步的处理
				if (detail::WMI_DoWithProperty(queryType, deviceProperty.szProperty, detail::DeviceProperty::PROPERTY_MAX_LEN)) {
					results[queryType] = (deviceProperty.szProperty);
					ok = true;
					pclsObj->Release();
					break;
				}
			}

			pclsObj->Release();
		} // End While
		  // 释放资源
		pEnumerator->Release();

	}

	pSvc->Release();
	pLoc->Release();
	CoUninitialize();
	return ok;
}

static std::wstring join_result(const std::vector<std::wstring>& results, const std::wstring & conjunction)
{
	std::wstring result;
	auto itBegin = results.cbegin();
	auto itEnd = results.cend();
	if (itBegin != itEnd) {
		result = *itBegin++;
	}
	for (; itBegin != itEnd; itBegin++) {
		result += conjunction;
		result += *itBegin;
	}
	return result;
}

}
}
