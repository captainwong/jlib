#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QT_LIB)
#  define QT_EXPORT Q_DECL_EXPORT
# else
#  define QT_EXPORT Q_DECL_IMPORT
# endif
#else
# define QT_EXPORT
#endif


#define ENABLE_JLIBQT_NAMESPACE

#ifdef ENABLE_JLIBQT_NAMESPACE
#define JLIBQT_NAMESPACE_BEGIN namespace jlib { namespace qt { 
#define JLIBQT_NAMESPACE_END } }
#define JLIBQT_NAMESPACE jlib::qt::
#else
#define JLIBQT_NAMESPACE_BEGIN
#define JLIBQT_NAMESPACE_END
#define JLIBQT_NAMESPACE
#endif
