#pragma once


#include "def.h"
#include <string>
#include <cstdint>

namespace jlib {
namespace win32 {
namespace info {
namespace baseboard {

// like: ASUSTeK COMPUTER INC.
JINFO_API std::string manufacturer();
// like: ROG STRIX B360-F GAMING
JINFO_API std::string product();
// like: "190449658701311" or "Default string" or "To be filled by O.E.M." or "None"
JINFO_API std::string serial();
// like: "416096H316A6240134" or "System Serial Number" or "To be filled by O.E.M."
JINFO_API std::string bios_serial();


}
}
}
}
