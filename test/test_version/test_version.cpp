#include "../../jlib/util/version.h"
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

	system("pause");
}
