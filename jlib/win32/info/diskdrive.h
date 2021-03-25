#pragma once

#include "def.h"
#include <string>
#include <cstdint>
#include <vector>

namespace jlib {
namespace win32 {
namespace info {
namespace diskdrive {

struct DiskDrive {
	int index = 0;
	int partitions{};
	std::string device_id{}; // like \\.\PHYSICALDRIVE0
	std::string model{}; // like Samsung SSD 840 EVO 250GB
	std::string serial{};
	std::uint64_t size{};
};

JINFO_API std::vector<DiskDrive> disk_drives();

JINFO_API std::string bootable_disk_serial();

}
}
}
}
