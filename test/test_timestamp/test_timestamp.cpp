#include "../../jlib/base/timestamp.h"
#include <vector>
#include <stdio.h>
#include <chrono>

using namespace jlib;

void passByConstReference(const Timestamp& x)
{
	printf("passByConstReference: %s\n", format("%F %T", x).c_str());
}

void passByValue(Timestamp x)
{
	printf("passByValue: %s\n", format("%F %T", x).c_str());
}

void benchmark()
{
	printf("benchmark\n");
	const int kNumber = 1000 * 1000;

	std::vector<Timestamp> stamps;
	stamps.reserve(kNumber);
	for (int i = 0; i < kNumber; ++i) {
		stamps.push_back(nowTimestamp());
	}
	printf("front %s\n", format("%F %T", stamps.front()).c_str());
	printf("back %s\n", format("%F %T", stamps.back()).c_str());
	printf("timeDifference %lld\n", (stamps.back() - stamps.front()).count());

	int increments[100] = { 0 };
	int64_t start = stamps.front().time_since_epoch().count();
	for (int i = 1; i < kNumber; ++i) {
		int64_t next = stamps[i].time_since_epoch().count();
		int64_t inc = next - start;
		start = next;
		if (inc < 0) {
			printf("reverse!\n");
		} else if (inc < 100) {
			++increments[inc];
		} else {
			printf("big gap %d\n", static_cast<int>(inc));
		}
	}

	for (int i = 0; i < 100; ++i) {
		printf("%2d: %d\n", i, increments[i]);
	}
}

int main()
{
	printf("sizeof Timestamp=%zd, sizeof int64_t=%zd\n", sizeof(Timestamp), sizeof(int64_t));
	Timestamp now(nowTimestamp());
	printf("now.toString(): %s\n", format("%F %T", now).c_str());
	passByValue(now);
	passByConstReference(now);
	benchmark();
}
