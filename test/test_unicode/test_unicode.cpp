#include "../../jlib/win32/UnicodeTool.h"
#include <stdio.h>
#include <assert.h>

using namespace jlib::win32;

int main()
{
	// gb2312 ansi
	// 不管cpp文件是什么编码，s1都是 CE D2 CA C7 D6 D0 B9 FA C8 CB
	char s1[] = "我是中国人"; 
	printf("s1 char:\n");
	printf("%s\n", s1); // 可以打印
	printf("raw %dbytes ", sizeof(s1) - 1);
	for (int i = 0; i < sizeof(s1) - 1; i++) {
		printf("%02X ", (unsigned char)s1[i]);
	}
	printf("\n\n");

	// 6211 662F 4E2D 56FD 4EBA
	wchar_t s2[] = L"我是中国人"; 
	printf("s2 wchar_t:\n");
	printf("%ls\n", s2); // 不可以打印
	printf("raw %dbytes ", sizeof(s1) - 1);
	for (int i = 0; i < sizeof(s2) / sizeof(wchar_t) - 1; i++) {
		printf("%04X ", (unsigned short)s2[i]);
	}
	printf("\n\n");

	// 6211 662F 4E2D 56FD 4EBA
	auto s1u16 = mbcs_to_utf16(s1);
	printf("s1u16 from mbcs_to_utf16 on s1:\n");
	printf("%ls\n", s1u16.data()); // 不可以打印
	printf("raw %zubytes ", s1u16.size() * sizeof(wchar_t));
	for (size_t i = 0; i < s1u16.size(); i++) {
		printf("%04X ", (unsigned short)s1u16[i]);
	}
	printf("\n\n");

	// E6 88 91 E6 98 AF E4 B8 AD E5 9B BD E4 BA BA
	auto s1u8 = mbcs_to_utf8(s1);
	printf("s1u8 from mbcs_to_utf8 on s1:\n");
	printf("%s\n", s1u8.data()); // 不可以打印
	printf("raw %zubytes ", s1u8.size() * sizeof(char));
	for (size_t i = 0; i < s1u8.size(); i++) {
		printf("%02X ", (unsigned char)s1u8[i]);
	}
	printf("\n\n");

	// CE D2 CA C7 D6 D0 B9 FA C8 CB
	auto s2mbcs = utf16_to_mbcs(s2);
	printf("s2mbcs from utf16_to_mbcs on s2:\n");
	printf("%s\n", s2mbcs.data()); // 可以打印
	printf("raw %zubytes ", s2mbcs.size() * sizeof(char));
	for (size_t i = 0; i < s2mbcs.size(); i++) {
		printf("%02X ", (unsigned char)s2mbcs[i]);
	}
	printf("\n\n");

	// E6 88 91 E6 98 AF E4 B8 AD E5 9B BD E4 BA BA
	auto s2u8 = utf16_to_utf8(s2);
	printf("s2u8 from utf16_to_utf8 on s2:\n");
	printf("%s\n", s2u8.data()); // 不可以打印
	printf("raw %zubytes ", s2u8.size() * sizeof(char));
	for (size_t i = 0; i < s2u8.size(); i++) {
		printf("%02X ", (unsigned char)s2u8[i]);
	}
	printf("\n\n");

	assert(s1u16 == s2);
	assert(s1u8 == s2u8);
	assert(s2mbcs == s1);

}
