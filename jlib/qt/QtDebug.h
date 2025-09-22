#pragma once

#include <QDebug>
#include <QDateTime>

#ifdef _WIN32
#include <Windows.h>
#endif //_WIN32


#define JLIBQT_QDEBUG_FILE_LINE_STREAM "[" << __FUNCTION__  << __LINE__ << "]: "
#define JLIBQT_QDEBUG_FILE_LINE_VALUE (QString("[") + __FUNCTION__ + " " + QString::number(__LINE__) + "]: ")

#define MYQDEBUG qDebug() << JLIBQT_QDEBUG_FILE_LINE_STREAM
#define MYQINFO qInfo() << JLIBQT_QDEBUG_FILE_LINE_STREAM
#define MYQWARN qWarning() << JLIBQT_QDEBUG_FILE_LINE_STREAM
#define MYQCRITICAL qCritical() << JLIBQT_QDEBUG_FILE_LINE_STREAM

#define MYQDEBUG_NOQUOTE qDebug().noquote() << JLIBQT_QDEBUG_FILE_LINE_STREAM
#define MYQINFO_NOQUOTE qInfo().noquote() << JLIBQT_QDEBUG_FILE_LINE_STREAM
#define MYQWARN_NOQUOTE qWarning().noquote() << JLIBQT_QDEBUG_FILE_LINE_STREAM
#define MYQCRITICAL_NOQUOTE qCritical().noquote() << JLIBQT_QDEBUG_FILE_LINE_STREAM

#define MYQDEBUG2 qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss.zzz") << JLIBQT_QDEBUG_FILE_LINE_STREAM
#define MYQINFO2 qInfo() << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss.zzz") << JLIBQT_QDEBUG_FILE_LINE_STREAM
#define MYQWARN2 qWarning() << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss.zzz") << JLIBQT_QDEBUG_FILE_LINE_STREAM
#define MYQCRITICAL2 qCritical() << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss.zzz") << JLIBQT_QDEBUG_FILE_LINE_STREAM

#define MYQDEBUG2_NOQUOTE qDebug().noquote() << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss.zzz") << JLIBQT_QDEBUG_FILE_LINE_STREAM
#define MYQINFO2_NOQUOTE qInfo().noquote() << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss.zzz") << JLIBQT_QDEBUG_FILE_LINE_STREAM
#define MYQWARN2_NOQUOTE qWarning().noquote() << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss.zzz") << JLIBQT_QDEBUG_FILE_LINE_STREAM
#define MYQCRITICAL2_NOQUOTE qCritical().noquote() << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss.zzz") << JLIBQT_QDEBUG_FILE_LINE_STREAM

#define MYQDEBUG3 qDebug().noquote() << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss.zzz")
#define MYQINFO3 qInfo().noquote() << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss.zzz")
#define MYQWARN3 qWarning().noquote() << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss.zzz")
#define MYQCRITICAL3 qCritical().noquote() << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss.zzz")

#define MYQDEBUG3_NOQUOTE qDebug().noquote() << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss.zzz")
#define MYQINFO3_NOQUOTE qInfo().noquote() << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss.zzz")
#define MYQWARN3_NOQUOTE qWarning().noquote() << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss.zzz")
#define MYQCRITICAL3_NOQUOTE qCritical().noquote() << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss.zzz")

//! 弹窗报告行号开关
// #define JLIBQT_SHOW_LINE 0
//! 当行号大于下方定义的值时，弹窗报告行号，否则忽略。可在多个cpp文件分别定义不同的值。
// #define JLIBQT_SHOW_MSGBOX_AFTER_LINE 1

#if JLIBQT_SHOW_LINE
#ifdef JLIBQT_SHOW_MSGBOX_AFTER_LINE
#define JLIBQT_SHOW_MSGBOX_LINE_NUMBER if(__LINE__ >= JLIBQT_SHOW_MSGBOX_AFTER_LINE) { MessageBoxW(nullptr, (std::wstring(__FILEW__) + L":" + std::to_wstring(__LINE__)).data(), L"Test", 0); }
#define JLIBQT_SHOW_MSGBOX_LINE_NUMBER_WITH_TITLE(title) if(__LINE__ >= JLIBQT_SHOW_MSGBOX_AFTER_LINE) { MessageBoxW(nullptr, (std::wstring(__FILEW__) + L":" + std::to_wstring(__LINE__)).data(), title, 0); }
#else
#define JLIBQT_SHOW_MSGBOX_LINE_NUMBER { MessageBoxW(nullptr, (std::wstring(__FILEW__) + L":" + std::to_wstring(__LINE__)).data(), L"Test", 0); }
#define JLIBQT_SHOW_MSGBOX_LINE_NUMBER_WITH_TITLE(title) { MessageBoxW(nullptr, (std::wstring(__FILEW__) + L":" + std::to_wstring(__LINE__)).data(), title, 0); }
#endif
#else
#define JLIBQT_SHOW_MSGBOX_LINE_NUMBER
#define JLIBQT_SHOW_MSGBOX_LINE_NUMBER_WITH_TITLE(title)
#endif
