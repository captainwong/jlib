#include "../../jlib/base/timestamp.h"
#include <vector>
#include <stdio.h>
#include <chrono>

using jlib::Timestamp;

void passByConstReference(const Timestamp& x)
{
	printf("passByConstReference: %s\n", x.toString().c_str());
}

void passByValue(Timestamp x)
{
	printf("passByValue: %s\n", x.toString().c_str());
}

void benchmark()
{
	printf("benchmark\n");
	const int kNumber = 1000 * 1000;

	std::vector<Timestamp> stamps;
	stamps.reserve(kNumber);
	for (int i = 0; i < kNumber; ++i) {
		stamps.push_back(Timestamp::now());
	}
	printf("front %s\n", stamps.front().toString().c_str());
	printf("back %s\n", stamps.back().toString().c_str());
	printf("timeDifference %f\n", timeDifference(stamps.back(), stamps.front()));

	int increments[100] = { 0 };
	int64_t start = stamps.front().microSecondsSinceEpoch();
	for (int i = 1; i < kNumber; ++i) {
		int64_t next = stamps[i].microSecondsSinceEpoch();
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
	Timestamp now(Timestamp::now());
	printf("now.toString(): %s\n", now.toString().c_str());
	printf("now.toFormattedString(): %s\n", now.toFormattedString().c_str());
	passByValue(now);
	passByConstReference(now);
	benchmark();
}
