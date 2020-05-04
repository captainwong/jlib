#pragma once

#include <Windows.h>
#include <string>
#include <vector>

namespace jlib {
namespace win32 {

struct MonitorInfo {
	struct Resolution {
		int w = 0, h = 0, hz = 0;

		bool operator==(const Resolution& r) const {
			return w == r.w && h == r.h && hz == r.hz;
		}

		std::wstring toString() const {
			std::wstring res = std::to_wstring(w) + L"x" + std::to_wstring(h)
				+ L"  " + std::to_wstring(hz) + L"Hz";
			return res;
		}

		bool valid() const { return !(w == 0 || h == 0 || hz == 0); }
	};

	bool isMain = false;
	std::wstring name = {};
	std::wstring deviceName = {};
	std::wstring deviceString = {};
	Resolution curResolution = {};
	std::vector<Resolution> supportResolutions = {};

	HMONITOR hMonitor = nullptr;
	HDC hDC = nullptr;
	RECT rc = {};

	bool isResolutionSupport(int w, int h, int hz) const {
		for (const auto& r : supportResolutions) {
			if (r.w == w && r.h == h && r.hz == hz) { return true; }
		}
		return false;
	}

	bool isResolutionSupport(const Resolution& r) const {
		for (const auto& l : supportResolutions) {
			if (l == r) { return true; }
		}
		return false;
	}

	bool valid() const {
		return curResolution.valid() && !name.empty() && !deviceName.empty() && !supportResolutions.empty();
	}

	std::wstring toString(bool withSupportRes = false) const {
		std::wstring str = L"{\n";
		str += L"  isMain        : "; str += isMain ? L"true" : L"false"; str += L"\n";
		str += L"  name          : " + name + L"\n";
		str += L"  deviceName    : " + deviceName + L"\n";
		str += L"  deviceString  : " + deviceString + L"\n";
		str += L"  hMonitor      : "; str += hMonitor ? L"valid" : L"nullptr"; str += L"\n";
		str += L"  hDC           : "; str += hDC ? L"valid" : L"nullptr"; str += L"\n";

		str += L"  rect          : "; 
		str += L"left=" + std::to_wstring(rc.left); 
		str += L" top=" + std::to_wstring(rc.top); 
		str += L" right=" + std::to_wstring(rc.right); 
		str += L" bottom=" + std::to_wstring(rc.bottom);
		str += L" (" + std::to_wstring(rc.right - rc.left) + L"x" + std::to_wstring(rc.bottom - rc.top) + L")\n";

		str += L"  curResolution : " + curResolution.toString() + L"\n";
		if (withSupportRes) {
			str += L"  supportResolutions : {\n";
			for (const auto& res : supportResolutions) {
				str += L"    " + res.toString() + L"\n";
			}
			str += L"  }\n";
		}
		str += L"}\n";
		return str;
	}
};

typedef std::vector<MonitorInfo> MonitorInfos;


static MonitorInfos getMonitors()
{
	MonitorInfos mis;
	BOOL ret = FALSE;
	DWORD iDevNum = 0;

	do {
		DISPLAY_DEVICE displayDevice;
		ZeroMemory(&displayDevice, sizeof(DISPLAY_DEVICE));
		displayDevice.cb = sizeof(displayDevice);
		ret = EnumDisplayDevicesW(NULL, iDevNum, &displayDevice, 0);

		//qDebug("StateFlags 0x%X",displayDevice.StateFlags);
		if (ret && (displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)) {
			//MYQDEBUG << "Device #" << iDevNum;
			//dumpDISPLAY_DEVICE(displayDevice);

			MonitorInfo mi;
			mi.deviceName = displayDevice.DeviceName;
			mi.deviceString = displayDevice.DeviceString;
			if (displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
				mi.isMain = true;
			}

			// get current resolution
			DEVMODE dm = { 0 };
			dm.dmSize = sizeof(dm);
			EnumDisplaySettingsW(displayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &dm);
			mi.curResolution.w = dm.dmPelsWidth;
			mi.curResolution.h = dm.dmPelsHeight;
			mi.curResolution.hz = dm.dmDisplayFrequency;

			// get supported resolutions
			for (int i = 0; 0 != EnumDisplaySettingsW(displayDevice.DeviceName, i, &dm); i++) {
				MonitorInfo::Resolution r{ (int)dm.dmPelsWidth, (int)dm.dmPelsHeight, (int)dm.dmDisplayFrequency };
				if (!mi.isResolutionSupport(r)) {
					mi.supportResolutions.push_back(r);
				}
			}

			if (!mi.supportResolutions.empty()) {
				// detached device, use first supportted res
				if (!mi.curResolution.valid()) {
					mi.curResolution = mi.supportResolutions.front();
				}				
			} else {
#ifdef _DEBUG
				//MYQDEBUG << "no supported resolutions!";
#endif
			}

			// get HMONITOR/HDC/etc.. by EnumDisplayMonitors
			EnumDisplayMonitors(NULL, NULL,
				[](HMONITOR hMonitor, HDC hDC, LPRECT rc, LPARAM data) -> BOOL {
					auto& mi = *reinterpret_cast<MonitorInfo*>(data);
					MONITORINFOEX info;
					info.cbSize = sizeof(info);
					if (GetMonitorInfoW(hMonitor, &info) && mi.deviceName == info.szDevice) {
						mi.hMonitor = hMonitor;
						mi.hDC = hDC;
						mi.rc = *rc;
						return FALSE;
					}
					return TRUE;
				},
				reinterpret_cast<LPARAM>(&mi));

			// get human readable name
			DISPLAY_DEVICE dd;
			ZeroMemory(&dd, sizeof(DISPLAY_DEVICE));
			dd.cb = sizeof(dd);
			int iMonitorNum = 0;
			if (EnumDisplayDevices(displayDevice.DeviceName, iMonitorNum, &dd, 0)) {
				//MYQDEBUG << "Device #" << iDevNum << "Monitor #" << iMonitorNum;
				//dumpDISPLAY_DEVICE(dd);
				std::wstring name = dd.DeviceString;
				if (name != L"Generic PnP Monitor" && name != L"通用即插即用监视器") {
					mi.name = name;
				} else {
					std::wstring deviceId = dd.DeviceID;
					deviceId = deviceId.substr(8, deviceId.find(L"\\", 9) - 8);
					mi.name = deviceId;
				}
				//dumpMoniterInfo(mi);
				mis.push_back(mi);
				ZeroMemory(&dd, sizeof(DISPLAY_DEVICE));
				dd.cb = sizeof(dd);
				iMonitorNum++;
			}
		}
		iDevNum++;
	} while (ret);

	return mis;
}

//! 获取主显示器信息
static MonitorInfo getMainMonitor(MonitorInfos mis = {}) {
	if (mis.empty()) { mis = getMonitors(); }
	for (const auto& mi : mis) { if (mi.isMain) { return mi; } }
	return MonitorInfo{};
}


//////////////////////////////////////////////////////////////////////////////////////////////


//! 设置显示模式：扩展模式
static bool setExtendMode() {
	auto ret = SetDisplayConfig(0, NULL, 0, NULL, SDC_TOPOLOGY_EXTEND | SDC_APPLY);
	//MYQDEBUG << "SetDisplayConfig SDC_TOPOLOGY_EXTEND returns:" << (ret == ERROR_SUCCESS);
	return ret == ERROR_SUCCESS;
}

//! 设置显示模式：克隆模式
static bool setCloneMode() {
	auto ret = SetDisplayConfig(0, NULL, 0, NULL, SDC_TOPOLOGY_CLONE | SDC_APPLY);
	//MYQDEBUG << "SetDisplayConfig SDC_TOPOLOGY_CLONE returns:" << (ret == ERROR_SUCCESS);
	return ret == ERROR_SUCCESS;
}

//void setOnlyDisplayInMainMonitor() {
//	auto ret = SetDisplayConfig(0, NULL, 0, NULL, SDC_TOPOLOGY_INTERNAL | SDC_APPLY);
//	MYQDEBUG << "SetDisplayConfig SDC_TOPOLOGY_EXTEND returns:" << ret;
//}
//
//void setOnlyDisplayInSecondMonitor() {
//	auto ret = SetDisplayConfig(0, NULL, 0, NULL, SDC_TOPOLOGY_EXTERNAL | SDC_APPLY);
//	MYQDEBUG << "SetDisplayConfig SDC_TOPOLOGY_EXTERNAL returns:" << ret;
//}

//! 设置后必须提交，否则不生效
static bool commitChange() {
	auto ret = ChangeDisplaySettingsExW(NULL, NULL, NULL, 0, NULL);
	//MYQDEBUG << "commitChange ChangeDisplaySettingsExW returns:" << (ret == ERROR_SUCCESS);
	return ret == ERROR_SUCCESS;
};

/*
* @brief 禁用某个显示器
* @param deviceName MonitorInfo::deviceName
*/
static bool disableMonitor(const std::wstring& deviceName) {
	DEVMODEW dm = { 0 };
	dm.dmSize = sizeof(dm);
	dm.dmFields = DM_POSITION | DM_PELSWIDTH | DM_PELSHEIGHT;
	dm.dmPelsWidth = dm.dmPelsHeight = 0;

	DWORD dwFlags = CDS_UPDATEREGISTRY | CDS_GLOBAL | CDS_NORESET;
	auto ret = ChangeDisplaySettingsExW(deviceName.data(), &dm, nullptr, dwFlags, nullptr);
	//MYQDEBUG << "disableMonitor ChangeDisplaySettingsExW returns:" << (ret == ERROR_SUCCESS);

	return commitChange();
}

/*
* @brief 设置主显示器
* @param newMain 新的主显示器
* @param oldMain 旧的主显示器
*/
static bool setMainMonitor(const MonitorInfo& newMain, const MonitorInfo& oldMain) {
	auto moveOld = [](const MonitorInfo& newMain, const MonitorInfo& oldMain) {
		DEVMODEW dm = { 0 };
		dm.dmSize = sizeof(dm);
		dm.dmFields = DM_POSITION;
		dm.dmPosition.x = newMain.curResolution.w;
		DWORD dwFlags = CDS_UPDATEREGISTRY | CDS_GLOBAL | CDS_NORESET;
		auto ret = ChangeDisplaySettingsExW(oldMain.deviceName.data(), &dm, nullptr, dwFlags, nullptr);
		//MYQDEBUG << "moveOld ChangeDisplaySettingsExW returns:" << (ret == ERROR_SUCCESS);
		return ret == ERROR_SUCCESS;
	};

	auto moveNew = [](const MonitorInfo& newMain) {
		DEVMODEW dm = { 0 };
		dm.dmSize = sizeof(dm);
		dm.dmFields = DM_POSITION;
		dm.dmPosition.x = dm.dmPosition.y = 0;

		DWORD dwFlags = CDS_UPDATEREGISTRY | CDS_GLOBAL | CDS_NORESET | CDS_SET_PRIMARY;

		auto ret = ChangeDisplaySettingsExW(newMain.deviceName.data(), &dm, nullptr, dwFlags, nullptr);
		//MYQDEBUG << "moveNew ChangeDisplaySettingsExW returns:" << (ret == ERROR_SUCCESS);
		return ret == ERROR_SUCCESS;
	};

	moveOld(newMain, oldMain);
	moveNew(newMain);
	return commitChange();
}

//! 修改显示器分辨率、刷新率
static bool changeResolution(const MonitorInfo& monitor, const MonitorInfo::Resolution& res) {
	DEVMODEW dm = { 0 };
	dm.dmSize = sizeof(dm);
	dm.dmPelsWidth = res.w;
	dm.dmPelsHeight = res.h;
	dm.dmDisplayFrequency = res.hz;
	dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
	DWORD dwFlags = CDS_UPDATEREGISTRY | CDS_GLOBAL;
	auto ret = ChangeDisplaySettingsExW(monitor.deviceName.data(), &dm, nullptr, dwFlags, nullptr);
	//MYQDEBUG << "changeResolution ChangeDisplaySettingsExW returns:" << (ret == ERROR_SUCCESS);
	return commitChange();
}


}
}
