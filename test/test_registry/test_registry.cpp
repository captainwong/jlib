#include "../../jlib/win32/registry.h"
#include <exception>
#include <iostream>

using namespace std;
using namespace jlib::win32::reg;

int main()
{
	constexpr int kExitError = 1;

	try
	{
		wcout << L"=========================================\n";
		wcout << L"*** Testing Giovanni Dicanio's WinReg ***\n";
		wcout << L"=========================================\n\n";

		//
		// Test subkey and value enumeration
		// 

		const wstring testSubKey = L"SOFTWARE\\GioTest";
		RegKey key;

		key.open(HKEY_CURRENT_USER, testSubKey);

		vector<wstring> subKeyNames = key.querySubKeys();
		wcout << L"Subkeys:\n";
		for (const auto& s : subKeyNames)
		{
			wcout << L"  [" << s << L"]\n";
		}
		wcout << L'\n';

		vector<pair<wstring, DWORD>> values = key.queryValues();
		wcout << L"Values:\n";
		for (const auto& v : values)
		{
			wcout << L"  [" << v.first << L"](" << RegKey::regTypeToString(v.second) << L")\n";
		}
		wcout << L'\n';

		key.close();


		//
		// Test SetXxxValue and GetXxxValue methods
		// 

		key.open(HKEY_CURRENT_USER, testSubKey);

		const DWORD testDw = 0x1234ABCD;
		const ULONGLONG testQw = 0xAABBCCDD11223344;
		const wstring testSz = L"CiaoTestSz";
		const wstring testExpandSz = L"%PATH%";
		const vector<BYTE> testBinary{ 0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33 };
		const vector<wstring> testMultiSz{ L"Hi", L"Hello", L"Ciao" };

		key.setDwordValue(L"TestValueDword", testDw);
		key.setQwordValue(L"TestValueQword", testQw);
		key.setStringValue(L"TestValueString", testSz);
		key.setExpandStringValue(L"TestValueExpandString", testExpandSz);
		key.setMultiStringValue(L"TestValueMultiString", testMultiSz);
		key.setBinaryValue(L"TestValueBinary", testBinary);

		DWORD testDw1 = key.getDwordValue(L"TestValueDword");
		if (testDw1 != testDw)
		{
			wcout << L"RegKey::GetDwordValue failed.\n";
		}

		DWORD typeId = key.queryValueType(L"TestValueDword");
		if (typeId != REG_DWORD)
		{
			wcout << L"RegKey::QueryValueType failed for REG_DWORD.\n";
		}

		ULONGLONG testQw1 = key.getQwordValue(L"TestValueQword");
		if (testQw1 != testQw)
		{
			wcout << L"RegKey::GetQwordValue failed.\n";
		}

		typeId = key.queryValueType(L"TestValueQword");
		if (typeId != REG_QWORD)
		{
			wcout << L"RegKey::QueryValueType failed for REG_QWORD.\n";
		}

		wstring testSz1 = key.getStringValue(L"TestValueString");
		if (testSz1 != testSz)
		{
			wcout << L"RegKey::GetStringValue failed.\n";
		}

		typeId = key.queryValueType(L"TestValueString");
		if (typeId != REG_SZ)
		{
			wcout << L"RegKey::QueryValueType failed for REG_SZ.\n";
		}

		wstring testExpandSz1 = key.getExpandStringValue(L"TestValueExpandString");
		if (testExpandSz1 != testExpandSz)
		{
			wcout << L"RegKey::GetExpandStringValue failed.\n";
		}

		typeId = key.queryValueType(L"TestValueExpandString");
		if (typeId != REG_EXPAND_SZ)
		{
			wcout << L"RegKey::QueryValueType failed for REG_EXPAND_SZ.\n";
		}

		vector<wstring> testMultiSz1 = key.getMultiStringValue(L"TestValueMultiString");
		if (testMultiSz1 != testMultiSz)
		{
			wcout << L"RegKey::GetMultiStringValue failed.\n";
		}

		typeId = key.queryValueType(L"TestValueMultiString");
		if (typeId != REG_MULTI_SZ)
		{
			wcout << L"RegKey::QueryValueType failed for REG_MULTI_SZ.\n";
		}

		vector<BYTE> testBinary1 = key.getBinaryValue(L"TestValueBinary");
		if (testBinary1 != testBinary)
		{
			wcout << L"RegKey::GetBinaryValue failed.\n";
		}

		typeId = key.queryValueType(L"TestValueBinary");
		if (typeId != REG_BINARY)
		{
			wcout << L"RegKey::QueryValueType failed for REG_BINARY.\n";
		}


		//
		// Remove some test values
		//

		key.deleteValue(L"TestValueDword");
		key.deleteValue(L"TestValueQword");
		key.deleteValue(L"TestValueString");
		key.deleteValue(L"TestValueExpandString");
		key.deleteValue(L"TestValueMultiString");
		key.deleteValue(L"TestValueBinary");



		wcout << L"All right!! :)\n\n";
	} catch (const RegException& e) {
		cout << "\n*** Registry Exception: " << e.what();
		cout << "\n*** [Windows API error code = " << e.errorCode();
		cout << "\n*** Description: " << e.description() << "\n\n";
		return kExitError;
	} catch (const exception& e) {
		cout << "\n*** ERROR: " << e.what() << '\n';
		return kExitError;
	}
}
