#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // _CRT_SECURE_NO_WARNINGS
#include "../../jlib/util/chrono_wrapper.h"
#include "../../jlib/win32/path_op.h"
#include <stdio.h>


using namespace jlib::win32;

int main()
{

	printf("getExePathA=%s\n", getExePathA().data());
	printf("getExeFolderPathA=%s\n", getExeFolderPathA().data());

	auto name = jlib::now_to_string(true) + ".txt";
	auto name2 = integrateFileName(name);
	printf("integrateFileName:\nold=%s\nnew=%s\n", name.data(), name2.data());

	printf("getAppDataPathA=%s\n", getAppDataPathA().data());
	printf("getTempFileName=%s\n", getTempFileNameA().data());

	system("pause");
}
