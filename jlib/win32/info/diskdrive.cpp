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
	if (wmi::WmiBase::simpleSelect({ L"DiskIndex", }, L"Win32_DiskPartition", L"Where BootPartition=True", result) && result.size() == 1) {
		auto index = result[0][L"DiskIndex"];
		result.clear();
		if (wmi::WmiBase::simpleSelect({ L"SerialNumber", }, L"Win32_DiskDrive", L"Where Index=" + index, result) && result.size() == 1) {
			return utf16_to_mbcs(result[0][L"SerialNumber"]);
		}
	}
	

	auto drives = disk_drives();
	if (!drives.empty()) {
		return drives[0].serial;
	}
	return {};
}

std::string bootable_disk_serial_old()
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
		return {};
	}

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
					//results[queryType] = values.back(); values.pop_back();
					//continue;
					return utf16_to_mbcs(values.back());
				}
			}
		}
	}

	return {};
}
}
}
}
}
