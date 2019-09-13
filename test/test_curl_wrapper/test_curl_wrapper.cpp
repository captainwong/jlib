#include "../../jlib/util/curl_wrapper.h"
#include <stdio.h>

#ifdef _WIN32
#pragma comment(lib, "../../jlib/3rdparty/curl/libcurldll.a")
#endif

using namespace jlib;

int main()
{
	Curl curl;
	curl.init();
	curl.get("qq.com");
	printf("HTTP %ld\n", curl.lastHttpCode());
	printf("%s\n", curl.lastHttpContent().data());
}
