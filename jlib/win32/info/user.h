#pragma once

#include "def.h"
#include <string>
#include <vector>

namespace jlib {
namespace win32 {
namespace info {
namespace user {

struct User {
	std::string name{};
	std::string full_name{};
	std::string privilege{};
};

JINFO_API std::vector<User> users();

}
}
}
}
