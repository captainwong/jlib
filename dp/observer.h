#pragma once

#include <memory>
#include <list>
#include <mutex>
#include <boost/noncopyable.hpp>

namespace dp {

template <typename target>
class observer : public std::enable_shared_from_this<observer<target>>
{
public:
	virtual void on_update(const target&) = 0;
};


template <typename target>
class observable : public boost::noncopyable
{
public:
	typedef observer<target> observer_type;
	typedef std::weak_ptr<observer_type> observer_ptr;
	typedef std::lock_guard<std::mutex> lock_guard_type;
protected:
	mutable std::mutex mutex_;
	std::list<observer_ptr> observers_;
public:
	void register_observer(const observer_ptr& obj) {
		lock_guard_type lock(mutex_);
		observers_.push_back(obj);
	}

	void notify_observers(const target& target) {
		lock_guard_type lock(mutex_);
		auto iter = observers_.begin();
		while (iter != observers_.end()) {
			std::shared_ptr<observer_type> obj(iter->lock());
			if (obj) {
				obj->on_update(target);
				++iter;
			} else {
				iter = observers_.erase(iter);
			}
		}
	}
};

};


