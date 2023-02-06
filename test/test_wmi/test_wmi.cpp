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
	/*{
		WmiBase wmi(L"root\\CIMV2", out, err);
		wmi.prepare();
		wmi.execute(L"SELECT SerialNumber FROM Win32_DiskDrive WHERE Index = 4");
		wmi.execute(L"SELECT * FROM Win32_Processor");
	}*/

	Result result;
	WmiBase::simpleSelect({ L"AdapterRAM", L"Description" }, L"Win32_VideoController", L"", result);
	for (const auto& i : result) {
		for (const auto& j : i) {
			printf("%ls %ls\n", j.first.data(), j.second.data());
		}
	}

	{

		//WmiBase wmi(L"root\\CIMV2", out, err);
		//wmi.prepare();
		//wmi.execute(L"SELECT SerialNumber FROM Win32_DiskDrive WHERE Index = 4");
		//wmi.execute(L"SELECT * FROM Win32_DiskPartition");
		//wmi.execute(L"SELECT Caption FROM Win32_BootConfiguration");
		//wmi.execute(L"Select Name from Win32_OperatingSystem");
		//wmi.execute(L"SELECT Description,AdapterRAM FROM Win32_VideoController");
		//wmi.execute(L"SELECT * FROM Win32_VideoController");
		//wmi.execute(L"SELECT PNPDeviceID FROM Win32_LogicalDisk WHERE NAME = 'C:'");
	}

	system("pause");
}
