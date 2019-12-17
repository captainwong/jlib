#pragma once

#include "config.h"
#include <stdint.h>

namespace jlib
{

#ifdef JLIB_WINDOWS
#include <Windows.h>
inline uint64_t getPid() {
	return GetCurrentProcessId();
}
#else
#include <sys/types.h>
#include <unistd.h>
inline uint64_t getPid() {
	return ::getpid();
}
#endif

}
