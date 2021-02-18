#pragma once

#include "../base/noncopyable.h"
#include <stdint.h>
#include <limits>
#include <queue>
#include <mutex>


namespace jlib {

// not thread safe
template <typename T>
class IdQueue : noncopyable
{
public:
	using value_type = T;

	explicit IdQueue(T min_val = 0, T max_val = std::numeric_limits<T>::max(), T init_hot = 50)
		: minValue(min_val)
		, maxValue(max_val)
		, currentMax(init_hot + min_val)
		, availableConnIds()
	{
		for (T i = 0; i < init_hot; i++) {
			availableConnIds.push(i + minValue);
		}		
	}

	virtual T aquire() {
		if (!availableConnIds.empty()) {
			auto id = availableConnIds.front();
			availableConnIds.pop();
			return id;
		} else if (currentMax < maxValue) {
			return currentMax++;
		} else {
			return maxValue; // failed
		}
	}

	virtual void release(T id) {
		availableConnIds.push(id);
	}

protected:
	T minValue = 0;
	T maxValue = 0;
	T currentMax = 0;
	std::queue<T> availableConnIds{};
};


// thread safe
template <typename T>
class IdQueue2 : public IdQueue<T>
{
public:
	explicit IdQueue2(T min_val = 0, T max_val = std::numeric_limits<T>::max(), T init_hot = 50)
		: IdQueue<T>(min_val, max_val, init_hot)
	{}

	virtual T aquire() override {
		std::lock_guard<std::mutex> lg(mutex);
		return IdQueue<T>::aquire();
	}

	virtual void release(T id) override {
		std::lock_guard<std::mutex> lg(mutex);
		IdQueue<T>::release(id);
	}

protected:
	std::mutex mutex{};
};

using IntIdQ = IdQueue<int>;
using IntIdQ2 = IdQueue2<int>;
using UInt32IdQ = IdQueue<uint32_t>;
using UInt32IdQ2 = IdQueue2<uint32_t>;
using UInt64IdQ = IdQueue<uint64_t>;
using UInt64IdQ2 = IdQueue2<uint64_t>;

}
