#include "../../jlib/base/timestamp.h"
#include <mutex>
#include <thread>
#include <vector>
#include <stdio.h>
#include <assert.h>

using namespace jlib;
using namespace std;

mutex g_mutex;
vector<int> g_vec;
const int kCount = 10 * 1000 * 1000;

void func() {
	for (int i = 0; i < kCount; i++) {
		lock_guard<mutex> lock(g_mutex);
		g_vec.push_back(i);
	}
}

int g_count = 0;
int foo() {
	lock_guard<mutex> lock(g_mutex);
	++g_count;
	return 0;
}

int main()
{
	assert(foo() == 0);
	if (g_count != 1) {
		printf("MCHECK calls twice.\n");
		abort();
	}

	const int kMaxThreads = 8;
	g_vec.reserve(kMaxThreads * kCount);

	Timestamp start(nowTimestamp());
	for (int i = 0; i < kCount; ++i) {
		g_vec.push_back(i);
	}

	printf("single thread without lock %lf\n", timeDifferenceInS(start));

	start = nowTimestamp();
	func();
	printf("single thread with lock %lf\n", timeDifferenceInS(start));

	for (int nthreads = 1; nthreads < kMaxThreads; ++nthreads) {
		std::vector<thread> threads;
		g_vec.clear();
		start = nowTimestamp();
		for (int i = 0; i < nthreads; ++i) {
			threads.emplace_back(func);
		}
		for (auto& t : threads) {
			t.join();
		}
		printf("%d thread(s) with lock %lf\n", nthreads, timeDifferenceInS(start));
	}
}
