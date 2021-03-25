#include "system.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define _WIN32_DCOM
#include <comdef.h>
#include <Wbemidl.h>
#include <memory>
#include <functional>
#include "../../util/space.h"
#include "../UnicodeTool.h"
#include "../wmi.h"

#pragma comment(lib, "wbemuuid.lib")
// require Api-ms-win-core-version-l1-1-0.dll
#pragma comment(lib, "Version.lib")


namespace jlib {
namespace win32 {
namespace info {
namespace system {

static std::string NT_get_value(const char* key)
{
	HKEY hkey;
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, R"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)", 0, KEY_READ | KEY_WOW64_64KEY, &hkey))
		return {};

	char value[4096] = { 0 };
	DWORD value_len = sizeof(value);
	LPBYTE lpdata = static_cast<LPBYTE>(static_cast<void*>(&value[0]));
	LSTATUS ret = RegQueryValueExA(hkey, key, nullptr, nullptr, lpdata, &value_len);
	if (ret != ERROR_SUCCESS) {
		RegCloseKey(hkey);
		return {};
	}
	RegCloseKey(hkey);
	return value;
}

static std::uint32_t NT_get_value_i(const char* key)
{
	HKEY hkey;
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, R"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)", 0, KEY_READ, &hkey))
		return {};

	DWORD value{};
	DWORD value_len = sizeof(value);
	if (RegGetValueA(hkey, nullptr, key, RRF_RT_REG_DWORD, nullptr, &value, &value_len)) {
		RegCloseKey(hkey);
		return {};
	}
	RegCloseKey(hkey);
	return value;
}

Memory memory() noexcept
{
	MEMORYSTATUSEX mem;
	mem.dwLength = sizeof(mem);
	if (!GlobalMemoryStatusEx(&mem))
		return {};

	return { mem.ullAvailPhys, mem.ullTotalPhys, mem.ullAvailVirtual, mem.ullTotalVirtual };
}

// Get OS (platform) version from kernel32.dll because GetVersion is deprecated in Win8+
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724439(v=vs.85).aspx
Version kernel_version()
{
	std::string path;
	path.resize(GetSystemDirectoryA(nullptr, 0) - 1);
	GetSystemDirectoryA(&path[0], static_cast<UINT>(path.size() + 1));
	path += "\\kernel32.dll";

	const auto ver_info_len = GetFileVersionInfoSizeA(path.c_str(), nullptr);
	auto ver_info = std::make_unique<std::uint8_t[]>(ver_info_len);
	GetFileVersionInfoA(path.c_str(), 0, ver_info_len, ver_info.get());

	VS_FIXEDFILEINFO* file_version;
	unsigned int file_version_len;
	VerQueryValueA(ver_info.get(), "", reinterpret_cast<void**>(&file_version), &file_version_len);

	return { HIWORD(file_version->dwProductVersionMS), LOWORD(file_version->dwProductVersionMS),
			HIWORD(file_version->dwProductVersionLS), LOWORD(file_version->dwProductVersionLS) };
}

std::string os_name()
{
	wmi::Result result;
	if (wmi::WmiBase::simpleSelect({ L"Name" }, L"Win32_OperatingSystem", L"", result) && result.size() == 1) {
		auto str = utf16_to_mbcs(result[0][L"Name"]);
		return str.substr(0, str.find('|'));
	}
	return {};
}

std::string computer_name()
{
	const auto INFO_BUFFER_SIZE = 4096;
	char  infoBuf[INFO_BUFFER_SIZE];
	DWORD  bufCharCount = INFO_BUFFER_SIZE;

	// Get and display the name of the computer.
	if (GetComputerNameA(infoBuf, &bufCharCount)) {
		return (infoBuf);
	}
	return {};
}

static BOOL IsWow64()
{
	BOOL bIsWow64 = FALSE;

	typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	//IsWow64Process is not available on all supported versions of Windows.
	//Use GetModuleHandle to get a handle to the DLL that contains the function
	//and GetProcAddress to get a pointer to the function if available.

	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (NULL != fnIsWow64Process)     {
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))         {
			//handle error
		}
	}
	return bIsWow64;
}

bool is_64bit_windows()
{
#if defined(_WIN64)
	return true;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
	return IsWow64();
#else
	return false; // Win64 does not support Win16
#endif
}

OsInfo os_info()
{
	return { "Windows NT", os_name(), kernel_version() };
}

std::string display_version()
{
	return NT_get_value("DisplayVersion");
}

std::string current_build()
{
	return NT_get_value("CurrentBuild");
}

std::uint32_t ubr()
{
	return NT_get_value_i("UBR");
}

std::string os_version()
{
	return current_build() + "." + std::to_string(ubr());
}

std::string system_info_string()
{
	auto info = os_name() + " ";
	const auto ki = kernel_version();
	info += std::to_string(ki.major) + "."
		+ std::to_string(ki.minor) + "."
		+ std::to_string(ki.patch) + "."
		+ std::to_string(ki.build_number) + " ";
	info += is_64bit_windows() ? "64bit" : "32bit";
	info += " Memory:";
	info += jlib::human_readable_byte_count(memory().physical_total);
	return info;
}

std::string product_id()
{
	return NT_get_value("ProductId");
}

std::string uuid()
{
	wmi::Result result;
	if (wmi::WmiBase::simpleSelect({ L"UUID" }, L"Win32_ComputerSystemProduct", L"", result) && result.size() == 1) {
		auto str = utf16_to_mbcs(result[0][L"UUID"]);
		return str;
	}
	return {};
}

}
}
}
}
