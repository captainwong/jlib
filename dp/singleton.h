#pragma once
#include <memory>
#include <mutex>
#include <boost/noncopyable.hpp>

namespace dp {


template <class T>
class singleton : public boost::noncopyable
{
protected:
	static std::shared_ptr<T> instance_;
	static std::mutex mutex_for_singleton_;
	singleton() {}

	class T_ctor : public T {
	public:
		T_ctor() : T() {}
	};

public:

	virtual ~singleton() {}

	static std::shared_ptr<T> get_instance() {
		std::lock_guard<std::mutex> lock(mutex_for_singleton_);
		if (!instance_) {
			instance_ = std::make_shared<T_ctor>();
		}
		return instance_;
	}

	static T& get_object() {
		return *get_instance();
	}

	static void release_singleton() {
		instance_ = nullptr;
	}

	static bool is_instance_exists() {
		return instance_ != nullptr;
	}
};


template <class T>
std::mutex singleton<T>::mutex_for_singleton_;

template <class T>
std::shared_ptr<T> singleton<T>::instance_ = nullptr;


}
