#pragma once

#include <string>
#include <vector>

namespace jlib
{
namespace win32
{
namespace DeviceUniqueIdentifier
{


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
bool query(size_t queryTypes, std::vector<std::wstring>& results);

/**
* @brief 查询信息
* @param[in] queryTypes QueryType集合
* @param[in,out] results 查询结果集合
* @return 成功或失败
*/
bool query(const std::vector<QueryType>& queryTypes, std::vector<std::wstring>& results);

/**
* @brief 连接查询结果为一个字符串
* @param[in] results 查询结果集合
* @param[in,out] conjunction 连词
* @return 将结果以连词连接起来组成的字符串
*/
std::wstring join_result(const std::vector<std::wstring>& results, const std::wstring& conjunction);

}
}
}
