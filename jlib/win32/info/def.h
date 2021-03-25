#pragma once

#if defined(_LIB) || defined(LINK_JINFO_STATIC_LIB)
#define JINFO_API
#else
#ifdef JINFO_EXPORTS
#define JINFO_API __declspec(dllexport)
#else
#define JINFO_API __declspec(dllimport)
#endif
#endif
