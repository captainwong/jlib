#pragma once

#include <QWidget>
#include <QPushButton>
#include <QToolButton>
#include <QLayout>
#include <QStyle>
#include <QLabel>
#include <QMouseEvent>
#include <QElapsedTimer>
#include <QLayout>
#include <QPixmap>
#include <QApplication>
#include "IconBtn.h"
#include "../QtStylesheet.h"

//namespace jlib
//{
//namespace qt
//{


class TitleBar : public QWidget
{
	Q_OBJECT

public:
	TitleBar(QWidget* parent = NULL,
			  QString minIcon = ":/Skin/system/min",
			  QString maxIcon = ":/Skin/system/max",
			  QString restoreIcon = ":/Skin/system/restore",
			  QString closeIcon = ":/Skin/system/close",
			  QString logoIcon = ":/Skin/favicon.png");

	~TitleBar();

public:
	void set_title(QString title);
	void set_maximize_btn_visible(bool visible);
	void set_maximized(bool isMax = true);


protected:
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;

signals:
	void sig_close();
	void sig_maximized(bool isMax);

private slots:
	void slot_minimize();
	void slot_maximize_or_restore();
	void slot_close();

private:
	IconBtn* minimize_ = {};
	IconBtn* maximize_ = {};
	IconBtn* close_ = {};
	QLabel* logo_ = {};
	QLabel* title_ = {};
	QHBoxLayout* layout_ = {};
	bool is_maximized_ = false;
	bool is_mouse_pressed_ = false;
	QPoint mouse_pressed_pos_ = {};
	QElapsedTimer lastTimeClick = {};

	QString minIcon_ = {};
	QString maxIcon_ = {};
	QString restoreIcon_ = {};
	QString closeIcon_ = {};
	QString logoIcon_ = {};
};

//}
//}
