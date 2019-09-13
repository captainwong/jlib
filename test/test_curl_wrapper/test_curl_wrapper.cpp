#include "../../jlib/util/curl_wrapper.h"
#include <stdio.h>


using namespace jlib;

int main()
{
	Curl curl;
	curl.init();
	curl.get("https://baidu.com");
	printf("HTTP %ld\n", curl.lastHttpCode());
	printf("error %d\n", curl.lastError());
	printf("error msg %s\n", curl.lastErrorMsg().data());
	printf("%s\n", curl.lastHttpContent().data());
}
