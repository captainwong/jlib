#include "diskdrive.h"
#include "../wmi.h"
#include "../UnicodeTool.h"
#include <algorithm>

namespace jlib {
namespace win32 {
namespace info {
namespace diskdrive {

std::vector<DiskDrive> disk_drives()
{
	std::vector<DiskDrive> drives;

	wmi::Result result;
	if (wmi::WmiBase::simpleSelect({ L"DeviceID", L"Index", L"Model", L"Partitions", L"SerialNumber", L"Size", }, L"Win32_DiskDrive", L"", result)) {
		for (auto& item : result) {
			DiskDrive drive;
			try {
				drive.index = std::stoi(item[L"Index"]);
				drive.partitions = std::stoi(item[L"Partitions"]);
				drive.size = std::stoull(item[L"Size"]);
				drive.device_id = utf16_to_mbcs(item[L"DeviceID"]);
				drive.model = utf16_to_mbcs(item[L"Model"]);
				drive.serial = utf16_to_mbcs(item[L"SerialNumber"]);
				drives.push_back(drive);
			} catch (...) {

			}
		}
	}

	std::sort(drives.begin(), drives.end(), [](const DiskDrive& d1, const DiskDrive& d2) {
		return d1.index < d2.index;
	});

	return drives;
}

std::string bootable_disk_serial()
{
	wmi::Result result;
	if (wmi::WmiBase::simpleSelect({ L"SerialNumber", }, L"Win32_DiskDrive", L"Where Index=0", result) && result.size() == 1) {
		return utf16_to_mbcs(result[0][L"SerialNumber"]);
	}

	auto drives = disk_drives();
	if (!drives.empty()) {
		return drives[0].serial;
	}
	return {};
}

}
}
}
}
