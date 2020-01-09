#include "../../jlib/win32/path_helper.h"

using namespace jlib::win32;

int main()
{
	PathHelperLocalWithoutBin helper;
	printf("exe=%s\n", helper.exe().data());
	printf("program=%s\n", helper.program().data());
	printf("bin=%s\n", helper.bin().data());
	printf("data=%s\n", helper.data().data());
	printf("config=%s\n", helper.config().data());
	printf("db=%s\n", helper.db().data());
	printf("log=%s\n", helper.log().data());
	printf("dumps=%s\n", helper.dumps().data());
}
