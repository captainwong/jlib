#pragma once

#include "wmi.h"
#include <winioctl.h> 
#include <ntddndis.h> // OID_802_3_PERMANENT_ADDRESS ...
#include <strsafe.h> // StringCch
#include <unordered_map>
#include <algorithm>

namespace jlib {
namespace win32 {
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
	因此，采用bootable（唯一）硬盘作为标识
	*/
	BOOTABLE_HARDDISK_SERIAL = 1 << 2,

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

static const wchar_t* queryTypeString(QueryType type)
{
	switch (type) {
	case QueryType::MAC_ADDR:					return L"MAC_ADDR                ";
	case QueryType::MAC_ADDR_REAL:				return L"MAC_ADDR_REAL           ";
	case QueryType::BOOTABLE_HARDDISK_SERIAL:	return L"BOOTABLE_HARDDISK_SERIAL";
	case QueryType::MOTHERBOARD_UUID:			return L"MOTHERBOARD_UUID        ";
	case QueryType::MOTHERBOARD_MODEL:			return L"MOTHERBOARD_MODEL       ";
	case QueryType::CPU_ID:						return L"CPU_ID                  ";
	case QueryType::BIOS_SERIAL:				return L"BIOS_SERIAL             ";
	case QueryType::WINDOWS_PRODUCT_ID:			return L"WINDOWS_PRODUCT_ID      ";
	case QueryType::WINDOWS_MACHINE_GUID:		return L"WINDOWS_MACHINE_GUID    ";
	default:									return L"UNKNOWN QueryType       ";
	}
}

enum {
	RecommendedQueryTypes = BOOTABLE_HARDDISK_SERIAL
		| MOTHERBOARD_UUID
		| MOTHERBOARD_MODEL
		| CPU_ID
		| BIOS_SERIAL
		| WINDOWS_PRODUCT_ID 
		| WINDOWS_MACHINE_GUID
		,

	AllQueryTypes = RecommendedQueryTypes
		| MAC_ADDR_REAL
		| MAC_ADDR
		,
};

typedef std::unordered_map<QueryType, std::wstring> QueryResults;

/**
* @brief 查询单个信息
* @param[in] queryType QueryType
* @return 查询结果
*/
static std::wstring query(QueryType type);


/**
* @brief 查询信息
* @param[in] queryTypes QueryType集合
* @param[in,out] results 查询结果集合
* @return 成功或失败
*/
static bool query(size_t queryTypes, QueryResults& results);

/**
* @brief 查询信息
* @param[in] queryTypes QueryType集合
* @param[in,out] results 查询结果集合
* @return 成功或失败
*/
static bool query(const std::vector<QueryType>& queryTypes, QueryResults& results);

/**
* @brief 连接查询结果为一个字符串
* @param[in] results 查询结果集合
* @param[in,out] conjunction 连词
* @return 将结果以连词连接起来组成的字符串
*/
static std::wstring join_result(const QueryResults& results, const std::wstring& conjunction);


// Implementation

namespace detail
{

inline const wchar_t* getWQLQuery(QueryType queryType) {
	switch (queryType) {
	case DeviceUniqueIdentifier::MAC_ADDR:
		return // 网卡当前MAC地址
			L"SELECT MACAddress FROM Win32_NetworkAdapter WHERE (MACAddress IS NOT NULL) AND (NOT (PNPDeviceID LIKE 'ROOT%'))" ;
	case DeviceUniqueIdentifier::MAC_ADDR_REAL:
		return // 网卡原生MAC地址
			L"SELECT PNPDeviceID FROM Win32_NetworkAdapter WHERE (MACAddress IS NOT NULL) AND (NOT (PNPDeviceID LIKE 'ROOT%'))";
	case DeviceUniqueIdentifier::MOTHERBOARD_UUID:
		return // 主板序列号
			L"SELECT SerialNumber FROM Win32_BaseBoard WHERE (SerialNumber IS NOT NULL)";
	case DeviceUniqueIdentifier::MOTHERBOARD_MODEL:
		return // 主板型号
			L"SELECT Product FROM Win32_BaseBoard WHERE (Product IS NOT NULL)";
	case DeviceUniqueIdentifier::CPU_ID:
		return // 处理器ID
			L"SELECT ProcessorId FROM Win32_Processor WHERE (ProcessorId IS NOT NULL)";
	case DeviceUniqueIdentifier::BIOS_SERIAL:
		return // BIOS序列号
			L"SELECT SerialNumber FROM Win32_BIOS WHERE (SerialNumber IS NOT NULL)";
	case DeviceUniqueIdentifier::WINDOWS_PRODUCT_ID:
		return // Windows 产品ID
			L"SELECT SerialNumber FROM Win32_OperatingSystem WHERE (SerialNumber IS NOT NULL)";
	case WINDOWS_MACHINE_GUID:
		return // 系统UUID
			L"SELECT UUID FROM Win32_ComputerSystemProduct";
		break;
	default:
		return L"" ;
	}
}

// 通过“PNPDeviceID”获取网卡原生MAC地址
static BOOL parsePNPDeviceID(const wchar_t* PNPDeviceID, std::wstring& macAddress)
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
			macAddress.clear();
			for (DWORD i = 0; i < dwByteRet; i++) {
				wchar_t tmp[4];
				swprintf_s(tmp, 4, i == dwByteRet - 1 ? L"%02X" : L"%02X:", ucData[i]);
				macAddress += tmp;
			}
		}
		CloseHandle(hDeviceFile);
	}
	return isOK;
}

static std::wstring getMachineGUID()
{
	std::wstring res;

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
		//res = utf8::a2w(e.what());
	}
	return res;
}

} // namespace detail


static bool query(size_t queryTypes, QueryResults& results)
{
	std::vector<QueryType> vec;

	if (queryTypes & MAC_ADDR) {
		vec.push_back(MAC_ADDR);
	}
	if (queryTypes & MAC_ADDR_REAL) {
		vec.push_back(MAC_ADDR_REAL);
	}
	if (queryTypes & BOOTABLE_HARDDISK_SERIAL) {
		vec.push_back(BOOTABLE_HARDDISK_SERIAL);
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
	return ok;
}

static std::wstring query(QueryType type)
{
	QueryResults results;
	auto ok = query(static_cast<size_t>(type), results);
	if (ok && !results.empty()) {
		return results.begin()->second;
	} else {
		return std::wstring(L"Query ") + queryTypeString(type) + L" failed";
	}
}

static bool query(const std::vector<QueryType>& queryTypes, QueryResults& results)
{
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

	for (auto queryType : queryTypes) {
		/*if (queryType == WINDOWS_MACHINE_GUID) {
			results[WINDOWS_MACHINE_GUID] = (detail::getMachineGUID());
			continue;
		} else */

		if (queryType == BOOTABLE_HARDDISK_SERIAL) {
			auto sz = values.size();

			// possible value:
			// Microsoft Windows 10 Pro|C:\WINDOWS|\Device\Harddisk4\Partition4
			if (wmi.execute(L"Select Name from Win32_OperatingSystem") && values.size() == sz + 1) {
				auto index = values.back(); values.pop_back();
				auto pos = index.find(L"Harddisk");
				if (pos != index.npos) {
					auto rpos = index.find(L"\\Partition", pos);
					if (rpos != index.npos && rpos - pos > strlen("Harddisk")) {
						auto n = index.substr(pos + strlen("Harddisk"), rpos - pos - strlen("Harddisk"));
						if (wmi.execute(L"SELECT SerialNumber FROM Win32_DiskDrive WHERE Index = " + n) && !values.empty()) {
							results[queryType] = values.back(); values.pop_back();
							continue;
						}
					}
				}
			}
		} else {
			auto wql = detail::getWQLQuery(queryType);
			auto sz = values.size();
			if (wmi.execute(wql) && values.size() > sz) {
				if (queryType == MAC_ADDR_REAL) {
					auto value = values.back(); values.pop_back();
					if (detail::parsePNPDeviceID(value.data(), value)) {
						results[queryType] = value;
					}
					while (values.size() > sz) {
						value = values.back(); values.pop_back();
						if (detail::parsePNPDeviceID(value.data(), value)) {
							results[queryType] += L"|" + value;
						}
					}
				} else {
					results[queryType] = values.back(); values.pop_back();
					while (values.size() > sz) {
						results[queryType] += L"|" + values.back(); values.pop_back();
					}
				}

				continue;
			}
		}

		// error
		if (!errors.empty()) {
			results[queryType] = errors.back(); errors.pop_back();
		} else {
			results[queryType] = std::wstring(L"Get ") + queryTypeString(queryType) + L" failed";
		}
	}

	return true;
}

static std::wstring join_result(const QueryResults& results, const std::wstring& conjunction)
{
	std::wstring result;
	auto itBegin = results.cbegin();
	auto itEnd = results.cend();
	if (itBegin != itEnd) {
		result = itBegin->second;
		itBegin++;
	}
	for (; itBegin != itEnd; itBegin++) {
		result += conjunction;
		result += itBegin->second;
	}
	return result;
}

} // namespace DeviceUniqueIdentifier 
} // namespace win32
} // namespace jlib
