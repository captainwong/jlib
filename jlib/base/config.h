#pragma once


#ifdef __GNUG__
#  define JLIB_LINUX
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#  define JLIB_WINDOWS
#else
#  error "jlib only support linux and windows"
#endif


#ifdef JLIB_WINDOWS

#  ifndef NOMINMAX
#    define NOMINMAX
#  endif

#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif

#  ifndef _CRT_SECURE_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
#  endif

// https://blogs.msdn.microsoft.com/vcblog/2016/03/30/optimizing-the-layout-of-empty-base-classes-in-vs2015-update-2-3/
#  define ENABLE_EBO __declspec(empty_bases)

#else // JLIB_WINDOWS

#  define ENABLE_EBO

#endif // JLIB_WINDOWS
