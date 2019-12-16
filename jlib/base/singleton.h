#pragma once

#include "noncopyable.h"
#include <mutex>
#include <stdlib.h>
#include <assert.h>

namespace jlib
{
    
namespace detail
{
    
// This doesn't detect inherited member functions!
// http://stackoverflow.com/questions/1966362/sfinae-to-check-for-inherited-member-functions
template <typename T>
struct has_no_destroy
{
    template <typename C> static char test(decltype(&C::no_destroy));
    template <typename C> static long test(...);
    static const bool value = sizeof(test<T>(nullptr)) == sizeof(char);
};

} // namespace detail


template <typename T>
class singleton : noncopyable
{
public:
    singleton() = delete;
    ~singleton() = delete;

    static T& instance() {
        std::call_once(once_, &singleton::init);
        assert(instance_ != nullptr);
        return *instance_;
    }

private:
    static void init() {
        instance_ = new T();
        if (!detail::has_no_destroy<T>::value) {
            ::atexit(destroy);
        }
    }

    static void destroy() {
        typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
        T_must_be_complete_type dummy; (void) dummy;
        
        delete instance_;
        instance_ = nullptr;
    }

private:
    static std::once_flag once_;
    static T* instance_;
};

template <typename T>
std::once_flag singleton<T>::once_ = {};

template <typename T>
T* singleton<T>::instance_ = nullptr;

} // namespace jlib
