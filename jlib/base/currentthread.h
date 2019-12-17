#pragma once

#include "config.h"
#include <inttypes.h>
#include <stdio.h>

#ifdef JLIB_LINUX
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#elif defined(JLIB_WINDOWS)
#include <Windows.h>
#endif

namespace jlib
{
    
namespace CurrentThread
{

namespace detail
{

inline uint64_t gettid() {
#ifdef JLIB_LINUX
    return static_cast<uint64_t>(::syscall(SYS_gettid));
#elif defined(JLIB_WINDOWS)
    return static_cast<uint64_t>(GetCurrentThreadId());
#endif
}

} // namespace detail


thread_local uint64_t t_cachedTid = 0;
thread_local char t_tidString[32] = {0};
thread_local int t_tidStringLength = 6;
thread_local const char* t_threadName = "unknown";

inline void cacheTid() {
    if (t_cachedTid == 0) {
        t_cachedTid = detail::gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%lld ", t_cachedTid);
    }
}

inline uint64_t tid() {
#ifdef JLIB_LINUX
    if (__builtin_expect(cachedTid == 0, 0)) {
        cachedTid();
    }
#else
    if (t_cachedTid != 0) {
        return t_cachedTid;
    } else {
        cacheTid();
    }
#endif

    return t_cachedTid;
}

inline const char* tidString() {
    return t_tidString;
}

inline int tidStringLength() {
    return t_tidStringLength;
}

inline const char* name() {
    return t_threadName;
}



} // namespace CurrentThread


} // namespace jlib
