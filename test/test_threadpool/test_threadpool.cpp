
#include "../../jlib/base/logging.h"
#include "../../jlib/base/threadpool.h"
#include "../../jlib/base/countdownlatch.h"
#include "../../jlib/base/currentthread.h"

using namespace jlib;
using namespace std::chrono;

void print() {
	printf("tid=%lld\n", CurrentThread::tid());
}

void printString(const std::string& str) {
	LOG_INFO << str;
	std::this_thread::sleep_for(100ms);
}

void test(int maxSize) {
	LOG_WARN << "Test ThreadPool with max queue size = " << maxSize;
	ThreadPool pool("MainThreadPool");
	pool.setMaxQueueSize(maxSize);
	pool.start(5);

	LOG_WARN << "Adding";
	pool.run(print);
	pool.run(print);

	for (int i = 0; i < 100; i++) {
		char buf[32];
		snprintf(buf, sizeof(buf), "task %d", i);
		pool.run(std::bind(printString, std::string(buf)));
	}

	LOG_WARN << "Done";

	CountDownLatch latch(1);
	pool.run(std::bind(&CountDownLatch::countDown, &latch));
	latch.wait();
	pool.stop();

	LOG_WARN << "All Done\n\n";
}

int main()
{
	Logger::setLogLevel(Logger::LOGLEVEL_DEBUG);
	LOG_INFO << _getpid();
	//test(0);
	//test(1);
	//test(5);
	//test(10);
	test(50);

}
