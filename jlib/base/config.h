#pragma once

#ifdef __GNUG__
#define JLIB_LINUX
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#define JLIB_WINDOWS
#else
#error "jlib only support linux and windows"
#endif

#ifdef JLIB_WINDOWS
#define _CRT_SECURE_NO_WARNINGS 
// https://blogs.msdn.microsoft.com/vcblog/2016/03/30/optimizing-the-layout-of-empty-base-classes-in-vs2015-update-2-3/
#define ENABLE_EBO __declspec(empty_bases)
#else
#define ENABLE_EBO
#endif
