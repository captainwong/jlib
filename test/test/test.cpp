// test.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS
#include "jlib/util/chrono_wrapper.h"
#include "jlib/dp.h"
#include "jlib/log2.h"
#include "jlib/util/mem_tool.h"
#include "jlib/util/micro_getter_setter.h"
//#include "jlib/net.h"
#include "jlib/util/space.h"
#include "jlib/util/std_util.h"
#include "jlib/utf8.h"
#include "jlib/util/version_no.h"
//#include "jlib/win32.h"


int main()
{
	jlib::init_logger(L"test");

	char data[0x80 - 0x20] = {};
	for (int i = 0x20; i < 0x80; i++) {
		data[i-0x20] = i;
	}

	JLOG_ASC(data, sizeof(data));
	JLOG_HEX(data, sizeof(data));

	data[0] = 31;
	JLOG_ASC(data, sizeof(data));


    return 0;
}

