#pragma once

#include "def.h"
#include <string>
#include <cstdint>
#include <vector>

namespace jlib {
namespace win32 {
namespace info {
namespace logicaldrive {

enum class DriveType {
	unknown,
	no_root_dir,
	removable,
	fixed,
	remote,
	//cdrom,
	ramdisk,
};

struct LogicalDrive {
	std::string device_id{}; // C:, D:, ...
	DriveType drive_type{};
	//std::string provider_name{}; 
	std::string volume_name{};
	std::uint64_t free_space{};
	std::uint64_t size{};
};

JINFO_API std::string drive_type_to_string(DriveType type);
JINFO_API std::vector<LogicalDrive> logical_drives();

}
}
}
}
