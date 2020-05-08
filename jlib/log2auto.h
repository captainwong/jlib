#pragma once 

#if defined(DISABLE_JLIB_LOG2) && !defined(JLIB_DISABLE_LOG)
#define JLIB_DISABLE_LOG
#endif

#ifndef JLIB_DISABLE_LOG
#include "log2.h"
#else // JLIB_DISABLE_LOG
#include "log2micros.h"
#endif // JLIB_DISABLE_LOG
