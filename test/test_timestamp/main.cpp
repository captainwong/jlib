#include <jlib/base/timestamp.h>
#include <vector>
#include <stdio.h>
#include <chrono>

using jlib::Timestamp;

void passByConstReference(const Timestamp& x)
{
	printf("%s\n", x.toString().c_str());
}

void passByValue(Timestamp x)
{
	printf("%s\n", x.toString().c_str());
}

void benchmark()
{
	const int kNumber = 1000 * 1000;

	std::vector<Timestamp> stamps;
	stamps.reserve(kNumber);
	for (int i = 0; i < kNumber; ++i)
	{
		stamps.push_back(Timestamp::now());
	}
	printf("%s\n", stamps.front().toString().c_str());
	printf("%s\n", stamps.back().toString().c_str());
	printf("%f\n", timeDifference(stamps.back(), stamps.front()));

	int increments[100] = { 0 };
	int64_t start = stamps.front().microSecondsSinceEpoch();
	for (int i = 1; i < kNumber; ++i)
	{
		int64_t next = stamps[i].microSecondsSinceEpoch();
		int64_t inc = next - start;
		start = next;
		if (inc < 0)
		{
			printf("reverse!\n");
		} else if (inc < 100)
		{
			++increments[inc];
		} else
		{
			printf("big gap %d\n", static_cast<int>(inc));
		}
	}

	for (int i = 0; i < 100; ++i)
	{
		printf("%2d: %d\n", i, increments[i]);
	}
}

struct __declspec(empty_bases) foo : public jlib::copyable
	, public boost::equality_comparable<foo>
{
public:
	void bar() {

	}

	static constexpr int64_t n = 0;
private:
	int64_t m;


};

void test_time()
{
	{
		time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	}
	{
		time_t t = 0;
		struct tm tm;
		gmtime_r(&t, &tm);

		time_t now = time(nullptr);
		gmtime_r(&now, &tm);
		tm.tm_isdst = -1;
		t = mktime(&tm);

		tm.tm_year += 1900;
		tm.tm_mon++;

		SYSTEMTIME st;
		GetSystemTime(&st);
	}

	// get epoch
	{
		SYSTEMTIME st;
		GetSystemTime(&st);
		st.wYear = 1970;
		st.wMonth = 1;
		st.wDay = 1;
		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		st.wMilliseconds = 0;

		struct tm tm = { 0 };
		tm.tm_year = 70;
		tm.tm_mon = 0;
		tm.tm_mday = 1;
		tm.tm_mday = 4;
		tm.tm_isdst = -1;

		time_t t = mktime(&tm);

		gmtime_r(&t, &tm);
		tm.tm_year += 1900;

	}

	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);

	LARGE_INTEGER large;
	QueryPerformanceCounter(&large); 
	Timestamp t(((large.QuadPart ) * 1000000 / Frequency.QuadPart));
	printf("%s\n", t.toFormattedString().c_str());

	/*FILETIME ft;
	GetSystemTimePreciseAsFileTime(&ft);
	ULARGE_INTEGER ularge;
	ularge.LowPart = ft.dwLowDateTime;
	ularge.HighPart = ft.dwHighDateTime;

	*/
}

int main()
{
	test_time();
	printf("sizeof foo=%zd, sizeof int64_t=%zd\n", sizeof(foo), sizeof(int64_t));
	printf("sizeof Timestamp=%zd, sizeof int64_t=%zd\n", sizeof(Timestamp), sizeof(int64_t));
	Timestamp now(Timestamp::now());
	printf("%s\n", now.toString().c_str());
	printf("%s\n", now.toFormattedString().c_str());
	passByValue(now);
	passByConstReference(now);
	benchmark();
}
