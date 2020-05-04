#include "../../jlib/win32/monitor.h"
#include <stdio.h>

using namespace jlib::win32;

int main()
{
	auto mis = getMonitors();
	printf("total %zu monitors:\n", mis.size());
	for (const auto& mi : mis) {
		wprintf(mi.toString().data());
	}
}
