#pragma once

#include <cassert>
#include <QString>
#include <QColor>
#include <QPixmap>
#include <QApplication>
#include <QDesktopwidget>
#include <QIcon>
#include <QTimer>
#include <QDir>
#include <QProcess>
#include "QtDebug.h"
#include "QtPathHelper.h"


namespace jlib {
namespace qt {

/**
* @brief 不阻塞UI响应的情况下，等待一段时间
* @param ms 等待的毫秒数
*/
static inline void non_blocking_wait_in_ui_thread(int ms) {
	QEventLoop q;
	QTimer t;
	t.setSingleShot(true);
	QObject::connect(&t, &QTimer::timeout, &q, &QEventLoop::quit);
	t.start(ms);
	q.exec();
}

/**
* @brief 在文件夹中显示
* @param pathIn 若为文件路径则在文件夹中显示，若为文件夹路径则打开改文件夹
*/
static inline void showInGraphicalShell(const QString &pathIn) {
	QString param;
	if (!QFileInfo(pathIn).isDir())
		param = QLatin1String("/select,");
	param += QDir::toNativeSeparators(pathIn);
	QString command = "explorer.exe " + param;
	QProcess::startDetached(command);
}

static inline bool warn_if_load_pixmap_failed(QPixmap& pixmap, QString icon_path, QString file_line)
{
	if (!QDir().isAbsolutePath(icon_path)) {
		icon_path = PathHelper::program() + "/" + icon_path;
	}
	
	if (!pixmap.load(icon_path) && !pixmap.load(icon_path, "png")) {
		qCritical() << file_line << "load pixmap failed: " << icon_path;
		assert(0);
		return false;
	}
	return true;
}

#define LOAD_PIXMAP_EX(icon_path) jlib::qt::warn_if_load_pixmap_failed(pixmap, icon_path, QDEBUG_FILE_LINE_VER)

static QIcon icon_from_path(QString path, QSize icon_sz) {
	QPixmap pixmap;
	LOAD_PIXMAP_EX(path);
	return QIcon(pixmap.scaled(icon_sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

static inline void fill_bg_with_color(QWidget * widget, QColor color)
{
	if (!widget) { return; }

	widget->setAutoFillBackground(true);
	QPalette palette;
	palette.setColor(QPalette::Background, color);
	widget->setPalette(palette);
}

static inline void set_pos(QWidget * widget, QRect pos)
{
	if (!widget) { return; }
	widget->setFixedSize(pos.width(), pos.height());
	widget->move(pos.left(), pos.top());
}

static inline void set_image_bg(QWidget* widget, QPixmap pixmap)
{
	if (!widget || pixmap.isNull())return;
	QPalette palette;
	palette.setBrush(QPalette::Window, QBrush(pixmap));
	widget->setPalette(palette);
}

static inline void set_image_bg(QWidget* widget, QString icon_path)
{
	QPixmap pixmap;
	if (!LOAD_PIXMAP_EX(icon_path)) { return; }
	set_image_bg(widget, pixmap);
}

static inline void center_to_parent(QWidget* widget, QWidget* parent)
{
	if (!widget || !parent) { return; }
	auto center = parent->rect().center();
	widget->move(center.x() - widget->width() / 2, center.y() - widget->height() / 2);
}

static inline void center_to_desktop(QWidget* widget)
{
	if (!widget) { return; }
	auto center = QApplication::desktop()->availableGeometry(widget).center();
	widget->move(center.x() - widget->width() / 2, center.y() - widget->height() / 2);
}

static inline void center_to_desktop(QWidget* widget, int new_width, int new_height)
{
	if (!widget) { return; }
	auto center = QApplication::desktop()->availableGeometry(widget).center();
	widget->setFixedSize(new_width, new_height);
	widget->move(center.x() - new_width / 2, center.y() - new_height / 2);
	widget->adjustSize();
}




}
}
