#include "config.h"
#include "noncopyable.h"
#include <mutex>

namespace jlib
{

class CountDownLatch : noncopyable
{
public:
	explicit CountDownLatch(int count)
		: mutex_()
		, cv_()
		, count_(count)
	{}

	void wait() {
		std::unique_lock<std::mutex> lock(mutex_);
		cv_.wait(lock, [this]() { return count_ <= 0; });
	}

	void countDown() {
		std::lock_guard<std::mutex> lock(mutex_);
		if (--count_ == 0) { cv_.notify_all(); }
	}

	int getCount() const {
		std::lock_guard<std::mutex> lock(mutex_);
		return count_;
	}

private:
	mutable std::mutex mutex_;
	std::condition_variable cv_;
	int count_;
};

}
