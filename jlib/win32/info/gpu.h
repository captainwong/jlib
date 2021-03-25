#pragma once

#include "def.h"
#include <string>
#include <cstdint>
#include <vector>

namespace jlib {
namespace win32 {
namespace info {
namespace gpu {

struct Gpu {
	std::string name{};
	std::uintmax_t memsz{};
};

JINFO_API std::vector<Gpu> gpus();

}
}
}
}
