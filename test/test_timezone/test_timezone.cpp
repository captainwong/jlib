#include <Windows.h>
#include <stdio.h>
#include "../../jlib/base/timezone.h"

int main()
{
	TIME_ZONE_INFORMATION info = { 0 };
	auto ret = GetTimeZoneInformation(&info);

	printf("info.bias=%ld\n", info.Bias);
}