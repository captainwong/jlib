#include <iostream>
#include "../../jlib/win32/file_op.h"

int main()
{
	std::wstring path;
	COMDLG_FILTERSPEC cf[2] = {};
	cf[0].pszName = L"Text Files";
	cf[0].pszSpec = L"*.txt";
	cf[1].pszName = L"All Files";
	cf[1].pszSpec = L"*.*";

	jlib::win32::getOpenFileDialogResult(path, nullptr, L"", L"txt", 2, cf);
	std::wcout << path << std::endl;

	system("pause");
}