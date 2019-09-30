#include "../../jlib/win32/clipboard.h"
#include <assert.h>
using namespace jlib::win32;

int main()
{
	std::wstring txt = L"Hello World!";

	to_clipboard(txt);

	auto txt2 = from_clipboard();

	assert(txt == txt2);
}