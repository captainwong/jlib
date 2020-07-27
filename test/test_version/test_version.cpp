#include "../../jlib/util/version.h"
#include "../../jlib/util/curl_wrapper.h"
#include "../../jlib/3rdparty/json/jsoncpp/json.h"
#include <stdio.h>
#include <assert.h>

using namespace jlib;

int main()
{
	// 0.0.0.0
	Version v1; 
	assert(!v1.valid());
	printf("default ctor: %s\n", v1.toString().data());

	// 0.0.0.1
	Version v2(0, 0, 0, 1); 
	assert(v2.valid()); 
	assert(v2 == Version("0.0.0.1"));
	printf("v2: %s\n", v2.toString().data());
	v2.toFile("v2.ini");

	// 0.0.0.1
	Version v3; v3.fromFile("v2.ini"); 
	assert(v2 == v3);
	printf("v3: %s\n", v3.toString().data());

	// 0.0.0.2
	Version v4(0, 0, 0, 2);
	assert(!(v3 == v4));
	assert(v3 < v4);
	assert(v3 <= v4);
	assert(!(v3 > v4));
	assert(!(v3 >= v4));
	assert(v4 > v3);
	assert(v4 >= v3);
	assert(!(v4 < v3));
	assert(!(v4 <= v3));

	v3 = "0.9.0.0";
	v4 = "1.0.0.0";
	assert(!(v3 == v4));
	assert(v3 < v4);
	assert(v3 <= v4);
	assert(!(v3 > v4));
	assert(!(v3 >= v4));
	assert(v4 > v3);
	assert(v4 >= v3);
	assert(!(v4 < v3));
	assert(!(v4 <= v3));


	Curl curl;
	curl.init();
	auto url = "https://jcenter.captainwong.cn/api/v1/apps/latest?name=test";
	curl.get(url);
	printf("HTTP %ld\n", curl.lastHttpCode());
	printf("error %d\n", curl.lastError());
	printf("error msg %s\n", curl.lastErrorMsg().data());
	printf("%s\n", curl.lastHttpContent().data());

	Json::Value value;	
	Json::Reader parser;
	if (parser.parse(curl.lastHttpContent(), value)) {
		LatestRelease latestRelease;
		resolveLatestRelease(value, latestRelease);
		printf("%s\n", latestRelease.toString().data());

	}

	system("pause");
}
