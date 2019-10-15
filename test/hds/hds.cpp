#include <jlib/win32/DeviceUniqueIdentifier.h>
#include <jlib/util/str_util.h>
#include <stdio.h>
#include <algorithm>
#include <locale.h>
#include <jlib/win32/wmi.h>
#include <unordered_set>


using namespace jlib;
using namespace jlib::win32::wmi;

struct disk {
	std::wstring DiskIndex = {};
	std::wstring BootPartition = {};
};

disk d;
std::unordered_map<std::wstring, std::wstring> disks;

void out2(const std::wstring& key, const std::wstring& value)
{
	static int i = 0;

	if (key == L"DiskIndex") {
		d.DiskIndex = value;
	} else {
		d.BootPartition = value;
	}

	if (++i % 2 == 0) {
		auto iter = disks.find(d.DiskIndex);
		if (iter != disks.end()) {
			if (iter->second != L"TRUE") {
				iter->second = d.BootPartition;
			}
		} else {
			disks[d.DiskIndex] = d.BootPartition;
		}
	}
}

void out3(const std::wstring& key, const std::wstring& value)
{
	auto s = value; trim(s);
	wprintf(L"%s \t = %s\n", key.data(), s.data());
}

void err(HRESULT hr, const std::wstring& msg)
{
	wprintf(L"Error 0x%08X, %s\n", hr, msg.data());
}

// won't work if more than one disk is "Active"

int main()
{
	{
		WmiBase wmi(L"root\\CIMV2", out2, err);
		wmi.prepare();
		wmi.execute(L"SELECT DiskIndex,BootPartition FROM Win32_DiskPartition");
	}

	{
		WmiBase wmi(L"root\\CIMV2", out3, err);
		wmi.prepare();

		std::vector<std::wstring> dup;
		for (auto r : disks) {
			dup.push_back(r.first);
		}
		std::sort(dup.begin(), dup.end());

		for (auto index : dup) {
			printf("\nDiskIndex\t = %ls ", index.data());
			if (disks[index] == L"TRUE") {
				printf("  <- BootPartition");
			}
			printf("\n");
			wmi.execute(L"SELECT Caption, SerialNumber FROM Win32_DiskDrive WHERE Index = " + index);
		}

	}

	system("pause");
}
