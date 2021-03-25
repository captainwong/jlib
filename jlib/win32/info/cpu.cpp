#include "cpu.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <intrin.h>
#include <bitset>
#include <array>
#include <vector>


namespace jlib {
namespace win32 {
namespace info {
namespace cpu {

struct cpu_id_helper {
    std::string vendor_;
    std::string brand_;
    std::string cpuid_;

	cpu_id_helper() {
        //int cpuInfo[4] = {-1};
        std::array<int, 4> cpui;
        std::vector<std::array<int, 4>> data_;
        std::vector<std::array<int, 4>> extdata_;

        // Calling __cpuid with 0x0 as the function_id argument
        // gets the number of the highest valid function ID.
        __cpuid(cpui.data(), 0);
        int nIds_ = cpui[0];
        for (int i = 0; i <= nIds_; ++i) {
            __cpuidex(cpui.data(), i, 0);
            data_.push_back(cpui);
        }

        // Capture vendor string
        char vendor[0x20];
        memset(vendor, 0, sizeof(vendor));
        *reinterpret_cast<int*>(vendor) = data_[0][1];
        *reinterpret_cast<int*>(vendor + 4) = data_[0][3];
        *reinterpret_cast<int*>(vendor + 8) = data_[0][2];
        vendor_ = vendor;

        char vendor_serialnumber[0x14] = { 0 };
        sprintf_s(vendor_serialnumber, sizeof(vendor_serialnumber), "%08X%08X", data_[1][3], data_[1][0]);
        cpuid_ = vendor_serialnumber;

        // Calling __cpuid with 0x80000000 as the function_id argument
        // gets the number of the highest valid extended ID.
        __cpuid(cpui.data(), 0x80000000);
        int nExIds_ = cpui[0];

        char brand[0x40];
        memset(brand, 0, sizeof(brand));

        for (int i = 0x80000000; i <= nExIds_; ++i) {
            __cpuidex(cpui.data(), i, 0);
            extdata_.push_back(cpui);
        }

        // Interpret CPU brand string if reported
        if (nExIds_ >= 0x80000004) {
            memcpy(brand, extdata_[2].data(), sizeof(cpui));
            memcpy(brand + 16, extdata_[3].data(), sizeof(cpui));
            memcpy(brand + 32, extdata_[4].data(), sizeof(cpui));
            brand_ = brand;
        }
	}
};

static const cpu_id_helper cpu_id_helper_;

Architecture architecture() noexcept
{
	SYSTEM_INFO si;
    GetNativeSystemInfo(&si);
	switch (si.wProcessorArchitecture) 	{
	case PROCESSOR_ARCHITECTURE_INTEL: return Architecture::x86;
	case PROCESSOR_ARCHITECTURE_AMD64: return Architecture::x64;
	case PROCESSOR_ARCHITECTURE_ARM:   return Architecture::arm;
	case PROCESSOR_ARCHITECTURE_ARM64: return Architecture::arm64;
	case PROCESSOR_ARCHITECTURE_IA64:  return Architecture::ia64;
	}
	return Architecture::unknown;
}

std::string architecture_to_string(Architecture arch)
{
    switch (arch) 	{
    case jlib::win32::info::cpu::Architecture::x86: return "x86";
    case jlib::win32::info::cpu::Architecture::x64: return "x64";
    case jlib::win32::info::cpu::Architecture::arm: return "ARM";
    case jlib::win32::info::cpu::Architecture::arm64: return "ARM64";
    case jlib::win32::info::cpu::Architecture::ia64: return "IA64";
    case jlib::win32::info::cpu::Architecture::unknown:
    default: return "unknown architecture";
    }
}

std::string cpu_id()
{
    return cpu_id_helper_.cpuid_;
}

std::string vendor()
{
	return cpu_id_helper_.vendor_;
}

std::string brand()
{
	return cpu_id_helper_.brand_;
}

std::string brand_registry()
{
    HKEY hkey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, R"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", 0, KEY_READ, &hkey))
        return {};

    char identifier[256] = { 0 };
    DWORD identifier_len = sizeof(identifier);
    LPBYTE lpdata = static_cast<LPBYTE>(static_cast<void*>(&identifier[0]));
    if (RegQueryValueExA(hkey, "ProcessorNameString", nullptr, nullptr, lpdata, &identifier_len)) {
        RegCloseKey(hkey);
        return {};
    }
    RegCloseKey(hkey);
    return identifier;
}

}
}
}
}
