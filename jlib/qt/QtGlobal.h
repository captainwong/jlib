#pragma once
#include <QDebug>
#ifdef _WIN32
#include <Windows.h>
#endif //_WIN32


#define QDEBUG_FILE_LINE_STREAM "[" << __FUNCTION__ << "ln" << __LINE__ << "]: "
#define QDEBUG_FILE_LINE_VER (QString("[") + __FUNCTION__ + " ln" + QString::number(__LINE__) + "]: ")

#define MYQDEBUG qDebug() << QDEBUG_FILE_LINE_STREAM
#define MYQWARN qWarning() << QDEBUG_FILE_LINE_STREAM
#define MYQCRITICAL qCritical() << QDEBUG_FILE_LINE_STREAM

// if not defined, disable redirect
// if defined, you must include log2.h before include this file
#define REDIRECT_QT_OUTPUT_TO_LOG


#ifdef REDIRECT_QT_OUTPUT_TO_LOG
#ifndef JLIB_LOG2_ENABLED
#error 'You must include <jlib/log2.h> first!'
#else // has JLIB_LOG2_ENABLED
static inline void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	using namespace jlib;
	QByteArray localMsg = msg.toLocal8Bit();
	switch (type) {
	case QtDebugMsg:
		JLOG_INFO(localMsg.data());
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
#endif // JLIB_LOG2_ENABLED
#endif // REDIRECT_QT_OUTPUT_TO_LOG
