#include "../../jlib/util/std_util.h"
#include <assert.h>

int main()
{
	// is_contain
	{
		std::string s = "12345";
		for (auto c : s) {
			assert(jlib::is_contain(s, c));
		}
	}

	// get_other
	{
		std::vector<int> All{ 1, 2, 3, 4, 5 };
		std::vector<int> Sub{ 1,2,3 };
		auto other = jlib::get_other(All, Sub);
		assert(other == std::vector<int>({ 4, 5 }));
	}

	// get_other
	{
		std::vector<std::wstring> all{ L"123", L"abc", L"def", L"ghi" };
		std::wstring sub(L"123");
		auto other = jlib::get_other(all, sub);
		assert(other == std::vector<std::wstring>({ L"abc", L"def", L"ghi" }));
	}

	// get_other
	{
		std::vector<std::string> all{ "123", "abc", "def", "ghi" };
		std::string sub("123");
		auto other = jlib::get_other(all, sub);
		assert(other == std::vector<std::string>({ "abc", "def", "ghi" }));
	}
}
