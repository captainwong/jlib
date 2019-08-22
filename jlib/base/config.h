#pragma once

#ifdef __GNUG__
#define JLIB_LINUX
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#define JLIB_WINDOWS
#else
#error "jlib only support linux and windows"
#endif
