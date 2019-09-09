#include <jlib/win32/DeviceUniqueIdentifier.h>
#include <stdio.h>
#include <algorithm>
#include <locale>

int main()
{
	std::locale::global(std::locale(""));
	using namespace jlib::DeviceUniqueIdentifier;
	std::vector<std::wstring> results;
	/*query(RecommendedQueryTypes, results);
	auto str = join_result(results, L"\n");
	std::wcout << str << std::endl;*/

	results.clear();
	query(AllQueryTypes, results);
	std::wstring str;
	std::vector<std::wstring> types = {
		L"MAC_ADDR 网卡MAC地址",
		L"MAC_ADDR_REAL 网卡原生MAC地址",
		L"HARDDISK_SERIAL 主硬盘序列号",
		L"MOTHERBOARD_UUID 主板序列号",
		L"MOTHERBOARD_MODEL 主板型号",
		L"CPU_ID 处理器序列号",
		L"BIOS_SERIAL BIOS序列号",
		L"WINDOWS_PRODUCT_ID Windows产品ID",
		L"WINDOWS_MACHINE_GUID Windows机器码",

	};
	for (size_t i = 0; i < std::min(types.size(), results.size()); i++) {
		printf("%ls:\n%ls\n\n", types[i].c_str(), results[i].c_str());
	}
	//auto str = join_result(results, L"\n");
	//std::wcout << std::endl << str << std::endl;

	system("pause");
}