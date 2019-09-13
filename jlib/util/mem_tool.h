#pragma once

#include <memory>
#include <functional>

namespace jlib {

template <typename T>
class auto_free
{
public:
    typedef std::function<void(T*)> deleter;

	explicit auto_free(T* data, deleter dter = {})
		: data_(data), deleter_(dter)
	{}

    ~auto_free() {
        if (data_ && deleter_) {
            deleter_(data_);
        }
    }

protected:
    T* data_ = nullptr;
	deleter deleter_ = nullptr;
};

}
