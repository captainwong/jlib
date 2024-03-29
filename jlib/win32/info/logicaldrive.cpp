#include "logicaldrive.h"
#include "../wmi.h"
#include "../UnicodeTool.h"

namespace jlib {
namespace win32 {
namespace info {
namespace logicaldrive {

std::string drive_type_to_string(DriveType type)
{
	switch (type) 	{
	case DriveType::no_root_dir:	return "no_root_dir";
	case DriveType::removable:		return "removable";
	case DriveType::fixed:			return "fixed";
	case DriveType::remote:			return "remote";
	//case DriveType::cdrom:			return "cdrom";
	case DriveType::ramdisk:		return "ramdisk";
	default:						return "unkown drive type";
	}
}

static DriveType parseDriveType(const std::wstring& str) {
	int type = std::stoi(str);
	switch (type) 	{
	case DRIVE_NO_ROOT_DIR: return DriveType::no_root_dir;
	case DRIVE_REMOVABLE:	return DriveType::removable;
	case DRIVE_FIXED:		return DriveType::fixed;
	case DRIVE_REMOTE:		return DriveType::remote;
	case DRIVE_RAMDISK:		return DriveType::ramdisk;
	default:				return DriveType::unknown;
	}
}

std::vector<LogicalDrive> logical_drives()
{
	std::vector<LogicalDrive> drives;

	wmi::Result devices;
	if (wmi::WmiBase::simpleSelect({ L"DeviceID" }, L"Win32_LogicalDisk", L"where DriveType != 5", devices)) {
		for (auto&& item : devices) {
			std::wstring DeviceID = item[L"DeviceID"];
			std::wstring Where = L"where DeviceID=\"" + DeviceID + L"\"";
			LogicalDrive drive;
			drive.device_id = utf16_to_mbcs(DeviceID);

			wmi::Result DriveType, FreeSpace, Size, VolumeName;
			if (!wmi::WmiBase::simpleSelect({ L"DriveType" }, L"Win32_LogicalDisk", Where, DriveType)) {
				continue;
			}
			drive.drive_type = parseDriveType(DriveType[0][L"DriveType"]);

			if (!wmi::WmiBase::simpleSelect({ L"FreeSpace" }, L"Win32_LogicalDisk", Where, FreeSpace)) {
				continue;
			}
			drive.free_space = FreeSpace.empty() ? 0 : std::stoull(FreeSpace[0][L"FreeSpace"]);

			if (!wmi::WmiBase::simpleSelect({ L"Size" }, L"Win32_LogicalDisk", Where, Size)) {
				continue;
			}
			drive.size = Size.empty() ? 0 : std::stoull(Size[0][L"Size"]);

			if (!wmi::WmiBase::simpleSelect({ L"VolumeName" }, L"Win32_LogicalDisk", Where, VolumeName)) {
				continue;
			}
			drive.volume_name = VolumeName.empty() ? "" : utf16_to_mbcs(VolumeName[0][L"VolumeName"]);
			drives.push_back(drive);
		}
	}

	/*if (wmi::WmiBase::simpleSelect({ L"DeviceID", L"DriveType", L"FreeSpace", L"Size", L"VolumeName" }, L"Win32_LogicalDisk", L"where DriveType != 5", result)) {
		for (auto&& item : result) {
			LogicalDrive drive;
			drive.device_id = utf16_to_mbcs(item[L"DeviceID"]);
			drive.drive_type = parseDriveType(item[L"DriveType"]);
			try {
				drive.free_space = std::stoull(item[L"FreeSpace"]);
				drive.size = std::stoull(item[L"Size"]);
			} catch (std::invalid_argument& e) {

			}
			drive.volume_name = utf16_to_mbcs(item[L"VolumeName"]);
			drives.push_back(drive);
		}
	}*/

	return drives;
}

}
}
}
}
