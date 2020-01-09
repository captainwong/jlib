#pragma once

#include "qt_global.h"
#include <qstring.h>
#include <Windows.h>
#include <qrect.h>
#include <vector>
#include <assert.h>
#include <qdebug.h>

JLIBQT_NAMESPACE_BEGIN

struct MonitorInfo {

	struct Resolution {
		int w;
		int h;
		int hz;

		bool operator==(const Resolution& r) const {
			return w == r.w && h == r.h && hz == r.hz;
		}

		QString toString() const {
			QString res = QString::number(w) + "X" + QString::number(h)
				+ "  " + QString::number(hz) + "HZ";
			return res;
		}

		bool valid() const {
			return !(w == 0 || h == 0 || hz == 0);
		}
	};

	bool isMain = false;
	QString name = {};
	QString deviceName = {};
	QString deviceString = {};
	Resolution curResolution = {};
	std::vector<Resolution> supportResolutions = {};

	bool isSupportResolutionsContain(int w, int h, int hz) {
		for (const auto& r : supportResolutions) {
			if (r.w == w && r.h == h && r.hz == hz) {
				return true;
			}
		}
		return false;
	}

	bool isSupportResolutionsContain(const Resolution& r) {
		for (const auto& l : supportResolutions) {
			if (l == r) {
				return true;
			}
		}
		return false;
	}

	bool valid() const {
		return curResolution.valid() && !name.isEmpty() && !deviceName.isEmpty() && !supportResolutions.empty();
	}
};

static void dumpMoniterInfo(const MonitorInfo& dd)
{
	QStringList list;
	for (auto res : dd.supportResolutions) {
		list << res.toString();
	}
#ifdef _DEBUG
	qDebug() << "dump MonitorInfo:\n"
		<< "isMain:\t" << dd.isMain << '\n'
		<< "name:\t" << dd.name << '\n'
		<< "DeviceName:\t" << dd.deviceName << '\n'
		<< "curResolution:\t" << dd.curResolution.toString() << '\n'
		<< "supportResolutions:\n" << list << '\n';
#endif
}

typedef std::vector<MonitorInfo> MonitorInfos;


static void dumpDISPLAY_DEVICE(const DISPLAY_DEVICE& dd)
{
#ifdef _DEBUG
	qDebug() << "dump DISPLAY_DEVICE:\n"
		<< "DeviceID:\t" << QString::fromWCharArray(dd.DeviceID) << '\n'
		<< "DeviceKey:\t" << QString::fromWCharArray(dd.DeviceKey) << '\n'
		<< "DeviceName:\t" << QString::fromWCharArray(dd.DeviceName) << '\n'
		<< "DeviceString:\t" << QString::fromWCharArray(dd.DeviceString) << '\n'
		<< "StateFlags:\t" << hex << dd.StateFlags << '\n'
		;
#endif
}


static void init_monitors(MonitorInfos& mis)
{
	std::vector<MonitorInfo::Resolution> _reses = {
						   {1024, 768, 60},
						   {1280, 720, 60},
						   {1280, 800, 60},
			   //#ifdef _DEBUG
						   {1360, 768, 60},
						   {1366, 768, 60},
			   //#endif
						   {1440, 900, 60},
						   {1920, 1080, 30},
						   {1920, 1080, 60},
						   {3840, 2160, 30},
						   {3840, 2160, 60},
	};

	auto is_in = [&_reses](int w, int h, int hz) {
		/*for (auto r : _reses) {
			if (r.w == w && r.h == h && r.hz == hz) {
				return true;
			}
		}
		return false;*/
		return true;
	};

	mis.clear();
	//EnumDisplayMonitors(NULL, nullptr, enum_display_monitors_cb, reinterpret_cast<LPARAM>(&mis));
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
			dumpDISPLAY_DEVICE(displayDevice);

			MonitorInfo mi;
			mi.deviceName = QString::fromWCharArray(displayDevice.DeviceName);
			mi.deviceString = QString::fromWCharArray(displayDevice.DeviceString);
			if (displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
				mi.isMain = true;
			}

			DEVMODE dm = { 0 };
			dm.dmSize = sizeof(dm);
			EnumDisplaySettings(displayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &dm);
			mi.curResolution.w = dm.dmPelsWidth;
			mi.curResolution.h = dm.dmPelsHeight;
			mi.curResolution.hz = dm.dmDisplayFrequency;

			for (int i = 0; 0 != EnumDisplaySettingsW(displayDevice.DeviceName, i, &dm); i++) {
				if (is_in(dm.dmPelsWidth, dm.dmPelsHeight, dm.dmDisplayFrequency)) {
					MonitorInfo::Resolution r;
					r.w = dm.dmPelsWidth;
					r.h = dm.dmPelsHeight;
					r.hz = dm.dmDisplayFrequency;

					if (!mi.isSupportResolutionsContain(r)) {
						mi.supportResolutions.push_back(r);
					}
				}
			}

			// detached device, use first supportted res
			if (!mi.supportResolutions.empty()) {
				if (!mi.curResolution.valid()) {
					mi.curResolution = mi.supportResolutions.front();
				}

				DISPLAY_DEVICE dd;
				ZeroMemory(&dd, sizeof(DISPLAY_DEVICE));
				dd.cb = sizeof(dd);
				int iMonitorNum = 0;
				if (EnumDisplayDevices(displayDevice.DeviceName, iMonitorNum, &dd, 0)) {
					//MYQDEBUG << "Device #" << iDevNum << "Monitor #" << iMonitorNum;
					//dumpDISPLAY_DEVICE(dd);

					auto name = QString::fromWCharArray(dd.DeviceString);
					if (name != "Generic PnP Monitor" && name != QString::fromLocal8Bit("通用即插即用监视器")) {
						mi.name = name;
					} else {
						std::wstring deviceId = dd.DeviceID;
						deviceId = deviceId.substr(8, deviceId.find(L"\\", 9) - 8);
						mi.name = QString::fromStdWString(deviceId);
					}

					//dumpMoniterInfo(mi);

					mis.push_back(mi);

					ZeroMemory(&dd, sizeof(DISPLAY_DEVICE));
					dd.cb = sizeof(dd);
					iMonitorNum++;
				}
			} else {
#ifdef _DEBUG
				MYQDEBUG << "no supported resolutions!";
#endif
				DISPLAY_DEVICE dd;
				ZeroMemory(&dd, sizeof(DISPLAY_DEVICE));
				dd.cb = sizeof(dd);
				int iMonitorNum = 0;
				if (EnumDisplayDevices(displayDevice.DeviceName, iMonitorNum, &dd, 0)) {
					//MYQDEBUG << "Device #" << iDevNum << "Monitor #" << iMonitorNum;
					//dumpDISPLAY_DEVICE(dd);

					auto name = QString::fromWCharArray(dd.DeviceString);
					if (name != "Generic PnP Monitor" && name != QString::fromLocal8Bit("通用即插即用监视器")) {
						mi.name = name;
					} else {
						std::wstring deviceId = dd.DeviceID;
						deviceId = deviceId.substr(8, deviceId.find(L"\\", 9) - 8);
						mi.name = QString::fromStdWString(deviceId);
					}

					//dumpMoniterInfo(mi);

					mis.push_back(mi);

					ZeroMemory(&dd, sizeof(DISPLAY_DEVICE));
					dd.cb = sizeof(dd);
					iMonitorNum++;
				}
			}
		}
		iDevNum++;
	} while (ret);

}

static void test_monitor()
{
	MonitorInfos mis;
	init_monitors(mis);

	for (const auto& mi : mis) {
		dumpMoniterInfo(mi);
	}
}

static void setExtendMode() {
	auto ret = SetDisplayConfig(0, NULL, 0, NULL, SDC_TOPOLOGY_EXTEND | SDC_APPLY);
	MYQDEBUG << "SetDisplayConfig SDC_TOPOLOGY_EXTEND returns:" << ret;
}

static void setCloneMode() {
	auto ret = SetDisplayConfig(0, NULL, 0, NULL, SDC_TOPOLOGY_CLONE | SDC_APPLY);
	MYQDEBUG << "SetDisplayConfig SDC_TOPOLOGY_CLONE returns:" << ret;
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

static void commitChange() {
	auto ret = ChangeDisplaySettingsExW(NULL, NULL, NULL, 0, NULL);
	MYQDEBUG << "commitChange ChangeDisplaySettingsExW returns:" << ret;
};

static void disableMonitor(const MonitorInfo& m) {
	DEVMODEW dm = { 0 };
	dm.dmSize = sizeof(dm);
	dm.dmFields = DM_POSITION | DM_PELSWIDTH | DM_PELSHEIGHT;
	dm.dmPelsWidth = dm.dmPelsHeight = 0;

	DWORD dwFlags = CDS_UPDATEREGISTRY | CDS_GLOBAL | CDS_NORESET;

	auto ret = ChangeDisplaySettingsExW(m.deviceName.toStdWString().data(), &dm, nullptr, dwFlags, nullptr);
	MYQDEBUG << "disableMonitor ChangeDisplaySettingsExW returns:" << ret;

	commitChange();
}

static void setMainMonitor(const MonitorInfo& newMain, const MonitorInfo& oldMain) {
	auto moveOld = [](const MonitorInfo& newMain, const MonitorInfo& oldMain) {
		DEVMODEW dm = { 0 };
		dm.dmSize = sizeof(dm);
		dm.dmFields = DM_POSITION;
		dm.dmPosition.x = newMain.curResolution.w;

		DWORD dwFlags = CDS_UPDATEREGISTRY | CDS_GLOBAL | CDS_NORESET;

		auto ret = ChangeDisplaySettingsExW(oldMain.deviceName.toStdWString().data(), &dm, nullptr, dwFlags, nullptr);
		MYQDEBUG << "moveOld ChangeDisplaySettingsExW returns:" << ret;
	};

	auto moveNew = [](const MonitorInfo& newMain) {
		DEVMODEW dm = { 0 };
		dm.dmSize = sizeof(dm);
		dm.dmFields = DM_POSITION;
		dm.dmPosition.x = dm.dmPosition.y = 0;

		DWORD dwFlags = CDS_UPDATEREGISTRY | CDS_GLOBAL | CDS_NORESET | CDS_SET_PRIMARY;

		auto ret = ChangeDisplaySettingsExW(newMain.deviceName.toStdWString().data(), &dm, nullptr, dwFlags, nullptr);
		MYQDEBUG << "moveNew ChangeDisplaySettingsExW returns:" << ret;
	};

	moveOld(newMain, oldMain);
	moveNew(newMain);
	commitChange();
}

static void changeResolution(const MonitorInfo& monitor, const MonitorInfo::Resolution& res) {
	DEVMODEW dm = { 0 };
	dm.dmSize = sizeof(dm);
	dm.dmPelsWidth = res.w;
	dm.dmPelsHeight = res.h;
	dm.dmDisplayFrequency = res.hz;
	dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
	DWORD dwFlags = CDS_UPDATEREGISTRY | CDS_GLOBAL;

	auto ret = ChangeDisplaySettingsExW(monitor.deviceName.toStdWString().data(), &dm, nullptr, dwFlags, nullptr);
	MYQDEBUG << "changeResolution ChangeDisplaySettingsExW returns:" << ret;

	commitChange();
}


#if 0

struct DisplayDevice {
	bool isMain = false;
	QString name = {};
	DISPLAY_DEVICEW displayDevice = {};
	DEVMODE devMode = {};
	QStringList supportResolutions = {};

	struct _res {
		int w;
		int h;
		int hz;
	};

	DisplayDevice() {
		ZeroMemory(&displayDevice, sizeof(displayDevice));
		displayDevice.cb = sizeof(displayDevice);

		ZeroMemory(&devMode, sizeof(devMode));
		devMode.dmSize = sizeof(devMode);
		devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
	}

	void genResolutions() {
		supportResolutions.clear();

		std::vector<_res> _reses = {
			{1024, 768, 60},
			{1280, 720, 60},
			{1280, 800, 60},
			{1920, 1080, 30},
			{1920, 1080, 60},
			{3840, 2160, 30},
			{3840, 2160, 60},
		};

		auto is_in = [&_reses](int w, int h, int hz) {
			for (auto r : _reses) {
				if (r.w == w && r.h == h && r.hz == hz) {
					return true;
				}
			}
			return false;
		};

		for (int i = 0; 0 != EnumDisplaySettingsW(displayDevice.DeviceName, i, &devMode); i++) {
			//qDebug() << "#" << i << ": " << devMode.dmPelsWidth << "X" << devMode.dmPelsHeight << devMode.dmDisplayFrequency;
			/*if (dm.dmPelsWidth == w && dm.dmPelsHeight == h && dm.dmDisplayFrequency == hz) {
				auto ret = ChangeDisplaySettingsW(&dm, CDS_GLOBAL | CDS_UPDATEREGISTRY);
				qDebug() << "ChangeDisplaySettings returns " << ret;
				return true;
			}*/
			if (is_in(devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmDisplayFrequency)) {
				QString res = QString::number(devMode.dmPelsWidth) + " X " + QString::number(devMode.dmPelsHeight)
					+ " " + QString::number(devMode.dmDisplayFrequency) + "HZ";
				if (!supportResolutions.contains(res)) {
					supportResolutions.push_back(res);
				}
			}
		}

		//MYQDEBUG << supportResolutions;
	}
};

void dumpDISPLAY_DEVICE(const DisplayDevice& dd)
{
	qDebug() << "dumpDISPLAY_DEVICE:\n"
		<< "isMain:\t" << dd.isMain << '\n'
		<< "name:\t" << dd.name << '\n'
		<< "DeviceID:\t" << QString::fromWCharArray(dd.displayDevice.DeviceID) << '\n'
		<< "DeviceKey:\t" << QString::fromWCharArray(dd.displayDevice.DeviceKey) << '\n'
		<< "DeviceName:\t" << QString::fromWCharArray(dd.displayDevice.DeviceName) << '\n'
		<< "DeviceString:\t" << QString::fromWCharArray(dd.displayDevice.DeviceString) << '\n'
		<< "supportResolutions:\n" << dd.supportResolutions << '\n';
}

std::list<DisplayDevice> dev_list;

{
	std::list<DisplayDevice> devices;
	int devId = 0;
	BOOL ret = FALSE;
	int PrimaryNum = 0;

	//list all DisplayDevices (Monitors)
	do {
		DisplayDevice displayDevice;

		ret = EnumDisplayDevicesW(NULL, devId, &displayDevice.displayDevice, 0);
		if (ret && (displayDevice.displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)) {
			if (displayDevice.displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
				PrimaryNum = devId;
				MYQDEBUG << "primary device:" << devId;
				displayDevice.isMain = true;
			}

			DISPLAY_DEVICE ddMon;
			ZeroMemory(&ddMon, sizeof(ddMon));
			ddMon.cb = sizeof(ddMon);
			if (EnumDisplayDevicesW(displayDevice.displayDevice.DeviceName, 0, &ddMon, 0)) {
				std::wstring deviceId = ddMon.DeviceID;
				deviceId = deviceId.substr(8, deviceId.find(L"\\", 9) - 8);
				displayDevice.name = QString::fromStdWString(deviceId);

				EnumDisplaySettingsW(displayDevice.displayDevice.DeviceName, ENUM_REGISTRY_SETTINGS, &displayDevice.devMode);
				displayDevice.genResolutions();
				if (!displayDevice.supportResolutions.empty()) {
					dumpDISPLAY_DEVICE(displayDevice);

#ifndef _DEBUG
					if (devices.size() < 2)
#endif
						devices.push_back(displayDevice);
				}
			}
		}
		devId++;
	} while (ret);
	dev_list = devices;
}


#endif

JLIBQT_NAMESPACE_END
