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
		assert(remove_all_copy(str, 'a') == "bbcc");
		assert(remove_all_copy(str, 'b') == "aacc");
		assert(remove_all_copy(str, 'c') == "aabb");

		remove_all(str, 'a'); assert(str == "bbcc");
		remove_all(str, 'b'); assert(str == "cc");
		remove_all(str, 'c'); assert(str == "");

		wstring wstr = L"aabbcc";
		assert(remove_all_copy(wstr, L'a') == L"bbcc");
		assert(remove_all_copy(wstr, L'b') == L"aacc");
		assert(remove_all_copy(wstr, L'c') == L"aabb");

		remove_all(wstr, L'a'); assert(wstr == L"bbcc");
		remove_all(wstr, L'b'); assert(wstr == L"cc");
		remove_all(wstr, L'c'); assert(wstr == L"");
	}
}