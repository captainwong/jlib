// test.cpp : Defines the entry point for the console application.
//

#include "jlib/chrono_wrapper.h"
#include "jlib/dp.h"
#include "jlib/log2.h"
#include "jlib/mem_tool.h"
#include "jlib/micro_getter_setter.h"
#include "jlib/net.h"
#include "jlib/space.h"
#include "jlib/std_util.h"
#include "jlib/utf8.h"
#include "jlib/version_no.h"
#include "jlib/vs_ver.h"
#include "jlib/win32.h"
#include <jlib/3rdparty/win32/Registry.hpp>
#include <jlib/win32/mfc/FileOper.h>


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

