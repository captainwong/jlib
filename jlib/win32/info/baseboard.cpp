#include "baseboard.h"
#include "../wmi.h"
#include "../UnicodeTool.h"

namespace jlib {
namespace win32 {
namespace info {
namespace baseboard {

static std::string BIOS_get_value(const char* key)
{
    HKEY hkey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, R"(HARDWARE\DESCRIPTION\System\BIOS)", 0, KEY_READ, &hkey))
        return {};

    char value[4096] = { 0 };
    DWORD value_len = sizeof(value);
    LPBYTE lpdata = static_cast<LPBYTE>(static_cast<void*>(&value[0]));
    if (RegQueryValueExA(hkey, key, nullptr, nullptr, lpdata, &value_len)) {
        RegCloseKey(hkey);
        return {};
    }
    RegCloseKey(hkey);
    return value;
}

std::string manufacturer()
{
    return BIOS_get_value("BaseBoardManufacturer");
}

std::string product()
{
    return BIOS_get_value("BaseBoardProduct");
}

std::string serial()
{
    wmi::Result result;
    if (wmi::WmiBase::simpleSelect({ L"SerialNumber" }, L"Win32_BaseBoard", L"", result) && result.size() == 1) {
        auto serial = result[0][L"SerialNumber"];
        return utf16_to_mbcs(serial);
    }
    return std::string("System Serial Number");
}

std::string bios_serial()
{
    wmi::Result result;
    if (wmi::WmiBase::simpleSelect({ L"SerialNumber" }, L"Win32_BIOS", L"", result) && result.size() == 1) {
        auto serial = result[0][L"SerialNumber"];
    }
    return std::string("System Serial Number");
}



}
}
}
}
