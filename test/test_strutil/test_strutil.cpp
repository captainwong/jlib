#include "../../jlib/util/str_util.h"
#include <assert.h>
#include <vector>
#include <list>

using namespace std;
using namespace jlib;

int main()
{

	// trim
	{
		string str = "   abcdef   ";
		assert(ltrim_copy(str) == "abcdef   ");
		assert(rtrim_copy(str) == "   abcdef");
		assert(trim_copy(str) == "abcdef");

		ltrim(str); assert(str == "abcdef   ");
		rtrim(str); assert(str == "abcdef");

		str = "   abcdef   ";
		trim(str); assert(str == "abcdef");
	}

	// join
	{
		vector<string> v = { "a", "b", "c" };
		list<string> l = { "d", "e", "f" };

		assert(join(v, string("-")) == "a-b-c");
		assert(join(l, string("-")) == "d-e-f");

		vector<wstring> wv = { L"a", L"b", L"c" };
		list<wstring> wl = { L"d", L"e", L"f" };

		assert(join(wv, wstring(L"-")) == L"a-b-c");
		assert(join(wl, wstring(L"-")) == L"d-e-f");
	}

	// remove_all
	{
		string str = "aabbcc";
		assert(erase_all_copy(str, 'a') == "bbcc");
		assert(erase_all_copy(str, 'b') == "aacc");
		assert(erase_all_copy(str, 'c') == "aabb");

		erase_all(str, 'a'); assert(str == "bbcc");
		erase_all(str, 'b'); assert(str == "cc");
		erase_all(str, 'c'); assert(str == "");

		wstring wstr = L"aabbcc";
		assert(erase_all_copy(wstr, L'a') == L"bbcc");
		assert(erase_all_copy(wstr, L'b') == L"aacc");
		assert(erase_all_copy(wstr, L'c') == L"aabb");

		erase_all(wstr, L'a'); assert(wstr == L"bbcc");
		erase_all(wstr, L'b'); assert(wstr == L"cc");
		erase_all(wstr, L'c'); assert(wstr == L"");
	}

	// case-conv
	{
		string str = "abc";
		assert(to_upper_copy(str) == "ABC");
		to_upper(str); assert(str == "ABC");
		assert(to_lower_copy(str) == "abc");
		to_lower(str); assert(str == "abc");

		wstring wstr = L"abc";
		assert(to_upper_copy(wstr) == L"ABC");
		to_upper(wstr); assert(wstr == L"ABC");
		assert(to_lower_copy(wstr) == L"abc");
		to_lower(wstr); assert(wstr == L"abc");

	}

	// starts_with
	{
		std::string str = "abc123";
		assert(starts_with(str, "abc123"));
		assert(starts_with(str, "abc"));
		assert(!starts_with(str, "abc1234"));
		assert(!starts_with(str, "bc"));

		std::wstring wstr = L"abc123";
		assert(starts_with(wstr, L"abc123"));
		assert(starts_with(wstr, L"abc"));
		assert(!starts_with(wstr, L"abc1234"));
		assert(!starts_with(wstr, L"bc"));
	}

	// ends_with
	{
		std::string str = "abc123";
		assert(ends_with(str, "abc123"));
		assert(ends_with(str, "123"));
		assert(!ends_with(str, "0abc123"));
		assert(!ends_with(str, "abc13"));

		std::wstring wstr = L"abc123";
		assert(ends_with(wstr, L"abc123"));
		assert(ends_with(wstr, L"123"));
		assert(!ends_with(wstr, L"0abc123"));
		assert(!ends_with(wstr, L"abc13"));
	}
}