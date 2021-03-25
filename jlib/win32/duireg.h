#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <intrin.h>
#include <string>
#include <array>
#include <vector>

// device unique identifier by registry

namespace jlib {
namespace win32 {
namespace duireg {

// https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=msvc-160
inline std::string cpu_id() {
    std::array<int, 4> cpui;
    std::vector<std::array<int, 4>> data_;

    __cpuid(cpui.data(), 0);
    int nIds_ = cpui[0];

    for (int i = 0; i <= nIds_; ++i) {
        __cpuidex(cpui.data(), i, 0);
        data_.push_back(cpui);
    }

    char serialnumber[0x14] = { 0 };
    sprintf_s(serialnumber, sizeof(serialnumber), "%08X%08X", data_[1][3], data_[1][0]);
    return serialnumber;
}

// CPU ÐÍºÅ
inline std::string cpu_model_name() {
    HKEY hkey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, R"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", 0, KEY_READ, &hkey))
        return {};

    char identifier[128] = { 0 };
    DWORD identifier_len = sizeof(identifier);
    LPBYTE lpdata = static_cast<LPBYTE>(static_cast<void*>(&identifier[0]));
    if (RegQueryValueExA(hkey, "ProcessorNameString", nullptr, nullptr, lpdata, &identifier_len)) {
        RegCloseKey(hkey);
        return {};
    }
    RegCloseKey(hkey);
    return identifier;
}

// Ö÷°åÐÍºÅ
inline std::string baseboard_product() {
    HKEY hkey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, R"(HARDWARE\DESCRIPTION\System\BIOS)", 0, KEY_READ, &hkey))
        return {};

    char identifier[128] = { 0 };
    DWORD identifier_len = sizeof(identifier);
    LPBYTE lpdata = static_cast<LPBYTE>(static_cast<void*>(&identifier[0]));
    if (RegQueryValueExA(hkey, "BaseBoardProduct", nullptr, nullptr, lpdata, &identifier_len)) {
        RegCloseKey(hkey);
        return {};
    }
    RegCloseKey(hkey);
    return identifier;
}

}
}
}
