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
	mutable std::mutex _mutex;
	std::list<observer_ptr> _observers;
public:
	void register_observer(const observer_ptr& obj) {
		lock_guard_type lock(_mutex);
		_observers.push_back(obj);
	}

	void notify_observers(const target& target) {
		lock_guard_type lock(_mutex);
		auto iter = _observers.begin();
		while (iter != _observers.end()) {
			std::shared_ptr<observer_type> obj(iter->lock());
			if (obj) {
				obj->on_update(target);
				++iter;
			} else {
				iter = _observers.erase(iter);
			}
		}
	}
};

};


