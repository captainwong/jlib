#include "gpu.h"
#include "../wmi.h"
#include "../UnicodeTool.h"

namespace jlib {
namespace win32 {
namespace info {
namespace gpu {

std::vector<Gpu> gpus()
{
	std::vector<Gpu> gs;
	wmi::Result result;
	if (wmi::WmiBase::simpleSelect({ L"AdapterRAM", L"Description" }, L"Win32_VideoController", L"", result)) {
		for (auto&& item : result) {
			Gpu gpu;
			gpu.name = utf16_to_mbcs(item[L"Description"]);
			gpu.memsz = std::stoull(item[L"AdapterRAM"]);
			gs.push_back(gpu);
		}
	}
	return gs;
}

}
}
}
}
