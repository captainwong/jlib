#include "../../jlib/win32/process.h"

using namespace jlib::win32;

int main()
{
	//printf("notepad exit code %d\n", daemon("notepad", true, true));
	//printf("notepad process id %d\n", daemon("notepad", false, true));
	printf("parent process id %d\n", getppid());
}
