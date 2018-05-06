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