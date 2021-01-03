#pragma once

#include "qt_global.h"

// if not defined, disable redirect
// if defined, you must include log2.h before include this file
// #define REDIRECT_QT_OUTPUT_TO_LOG


#ifdef JLIBQT_REDIRECT_QT_OUTPUT_TO_JLOG
#ifndef JLIB_LOG2_ENABLED
#error 'You must include <jlib/log2.h> first!'
#else // has JLIB_LOG2_ENABLED
JLIBQT_NAMESPACE_BEGIN
static inline void JLIBQT_MessageOutputFunc(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	using namespace jlib;
	QByteArray localMsg = msg.toLocal8Bit();
	switch (type) {
	case QtDebugMsg:
		JLOG_DBUG(localMsg.data());
		break;
	case QtWarningMsg:
		if (!msg.contains("libpng")) {
			JLOG_WARN(localMsg.data());
		}
		break;
	case QtCriticalMsg:
		JLOG_CRTC(localMsg.data());
		break;
	case QtFatalMsg:
		JLOG_ERRO(localMsg.data());
		break;
	case QtInfoMsg:
		JLOG_INFO(localMsg.data());
		break;
	default:
		JLOG_WARN(localMsg.data());
		break;
	}
}
JLIBQT_NAMESPACE_END
#endif // JLIB_LOG2_ENABLED
#endif // JLIBQT_REDIRECT_QT_OUTPUT_TO_JLOG
