#define _CRT_SECURE_NO_WARNINGS
#include "../../jlib/log2.h"
#include "../../jlib/util/curl_wrapper.h"
#include <stdio.h>


using namespace jlib;

int main()
{
	jlib::init_logger(L"curl");

	Curl curl;
	curl.init();

	/*curl.get("https://baidu.com");
	printf("HTTP %ld\n", curl.lastHttpCode());
	printf("error %d\n", curl.lastError());
	printf("error msg %s\n", curl.lastErrorMsg().data());
	printf("%s\n", curl.lastHttpContent().data());*/

	const char* json = R"({
    "items": [
        {
            "account": "861234567890",
            "online": true,
            "ops": [
                {
                    "event": 3400,
                    "source": 0,
                    "timestamp": 1568401523
                }
            ],
            "passwd": "123456",
            "status": 0,
            "type": 7
        }
    ],
    "sign": "sss",
    "ver": "1.0"
})";

	auto url = "127.0.0.1:8000";
	curl.postJson(url, json);
	printf("HTTP %ld\n", curl.lastHttpCode());
	printf("error %d\n", curl.lastError());
	printf("error msg %s\n", curl.lastErrorMsg().data());
	printf("%s\n", curl.lastHttpContent().data());
}
