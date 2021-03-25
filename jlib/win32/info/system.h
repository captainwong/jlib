#pragma once

#include "def.h"
#include <cstdint>
#include <string>

namespace jlib {
namespace win32 {
namespace info {
namespace system {

struct Memory {
	std::uint64_t physical_available = 0;
	std::uint64_t physical_total = 0;
	std::uint64_t virtual_available = 0;
	std::uint64_t virtual_total = 0;
};

struct Version {
	std::uint32_t major = 0;
	std::uint32_t minor = 0;
	std::uint32_t patch = 0;
	std::uint32_t build_number = 0;
};

struct OsInfo {
	std::string name{};
	std::string full_name{};
	Version version{};
};

JINFO_API Memory memory() noexcept;
JINFO_API Version kernel_version();
// like: Microsoft Windows 7 家庭普通版
JINFO_API std::string os_name();
// like: DESKTOP-HK8EHO
JINFO_API std::string computer_name();
// check OS is 32bit or 64bit
JINFO_API bool is_64bit_windows();

// { "Windows NT", os_name(), kernel_version() }
JINFO_API OsInfo os_info();

// like 20H2
JINFO_API std::string display_version();

// like 19042
JINFO_API std::string current_build();
// like 867
JINFO_API std::uint32_t ubr();
// 19042.867
JINFO_API std::string os_version();

// one line description like:
// Microsoft Windows 7 家庭普通版  6.1.7601.17514 32bit Memory:3.9995GiB
JINFO_API std::string system_info_string();

JINFO_API std::string product_id();
JINFO_API std::string uuid();

}
}
}
}
