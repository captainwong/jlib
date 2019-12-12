#include "../../jlib/base/logstream.h"
#include "../../jlib/base/timestamp.h"
#include <stdio.h>

using namespace jlib;


const size_t N = 1000000;


template<typename T>
void benchPrintf(const char* fmt)
{
	char buf[32];
	Timestamp start(nowTimestamp());
	for (size_t i = 0; i < N; ++i)
		snprintf(buf, sizeof buf, fmt, (T)(i));
	Timestamp end(nowTimestamp());

	printf("benchPrintf %s\n", format("%S", (end - start)).c_str());
}

template<typename T>
void benchStringStream()
{
	Timestamp start(nowTimestamp());
	std::ostringstream os;

	for (size_t i = 0; i < N; ++i) {
		os << (T)(i);
		os.seekp(0, std::ios_base::beg);
	}
	Timestamp end(nowTimestamp());

	printf("benchStringStream %s\n", format("%S", (end - start)).c_str());
}

template<typename T>
void benchLogStream()
{
	Timestamp start(nowTimestamp());
	LogStream os;
	for (size_t i = 0; i < N; ++i) {
		os << (T)(i);
		os.resetBuffer();
	}
	Timestamp end(nowTimestamp());

	printf("benchLogStream %s\n", format("%S", (end - start)).c_str());
}

int main()
{
	benchPrintf<int>("%d");

	puts("int");
	benchPrintf<int>("%d");
	benchStringStream<int>();
	benchLogStream<int>();

	puts("double");
	benchPrintf<double>("%.12g");
	benchStringStream<double>();
	benchLogStream<double>();

	puts("int64_t");
	benchPrintf<int64_t>("%" PRId64);
	benchStringStream<int64_t>();
	benchLogStream<int64_t>();

	puts("void*");
	benchPrintf<void*>("%p");
	benchStringStream<void*>();
	benchLogStream<void*>();

}
