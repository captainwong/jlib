#include "../../jlib/win32/wmi.h"

using namespace jlib::win32::wmi;

void out(const std::wstring& key, const std::wstring& value)
{
	wprintf(L"%s\t%s\n", key.data(), value.data());
}

void err(HRESULT hr, const std::wstring& msg)
{
	wprintf(L"Error 0x%08X, %s\n", hr, msg.data());
}

int main()
{
	{
		WmiBase wmi(L"root\\CIMV2", out, err);
		wmi.prepare();
		wmi.execute(L"SELECT SerialNumber FROM Win32_DiskDrive WHERE Index = 4");
		wmi.execute(L"SELECT * FROM Win32_Processor");
	}

	{

		WmiBase wmi(L"root\\CIMV2", out, err);
		wmi.prepare();
		wmi.execute(L"SELECT SerialNumber FROM Win32_DiskDrive WHERE Index = 4");
		wmi.execute(L"SELECT * FROM Win32_Processor");
	}
}
