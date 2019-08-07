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

namespace jlib
{
namespace qt
{

class PageTitle : public QWidget
{
	Q_OBJECT

public:
	PageTitle(QWidget *parent, QString logo_path)
		: QWidget(parent)
	{
		setAutoFillBackground(true);

		QPalette pal = palette();
		pal.setColor(QPalette::Window, Qt::black);
		setPalette(pal);

		minimize_ = new IconBtn(this, "Skin/system/min");
		maximize_ = new IconBtn(this, "Skin/system/max");
		close_ = new IconBtn(this, "Skin/system/close");

		connect(minimize_, SIGNAL(clicked()), this, SLOT(slot_minimize()));
		connect(maximize_, SIGNAL(clicked()), this, SLOT(slot_maximize_or_restore()));
		connect(close_, SIGNAL(clicked()), this, SLOT(slot_close()));

		logo_ = new QLabel(this);
		QPixmap pixmap;
		pixmap.load(logo_path);
		logo_->setFixedSize(32, 32);
		logo_->setPixmap(pixmap.scaled(32, 32, Qt::KeepAspectRatio));

		title_ = new QLabel(this);
		//title_->setStyleSheet(def_style_sheets::title);
		title_->setStyleSheet(build_style(def_colors::font_normal3, 18));

		layout_ = new QHBoxLayout(this);
		layout_->addWidget(logo_);
		layout_->addSpacing(5);
		layout_->addWidget(title_);
		layout_->addStretch();
		layout_->addWidget(minimize_);
		layout_->addWidget(maximize_);
		layout_->addWidget(close_);
		layout_->setSpacing(0);

		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		setMinimumHeight(7);

		lastTimeClick.start();
	}

public:
	void set_title(QString title) {
		title_->setText(title);
	}
	void set_maximize_btn_visible(bool visible) {
		visible ? maximize_->show() : maximize_->hide();
	}
	void set_maximized(bool isMax = true) {
		is_maximized_ = isMax;
		if (isMax) {
			maximize_->set_icon_path("Skin/system/restore");
			parentWidget()->showMaximized();
		} else {
			maximize_->set_icon_path("Skin/system/max");
			parentWidget()->showNormal();
		}
	}

protected:
	virtual void mousePressEvent(QMouseEvent* e) override {
		if (!rect().contains(e->pos()))return;
		is_mouse_pressed_ = true;
		mouse_pressed_pos_ = mapToParent(e->pos());
	}

	virtual void mouseMoveEvent(QMouseEvent* e) override {
		if (!is_mouse_pressed_)return;
		if (is_maximized_) {
			maximize_->set_icon_path("Skin/system/max");
			parentWidget()->showNormal();
			is_maximized_ = false;
		}
		parentWidget()->move(e->globalPos() - mouse_pressed_pos_);
	}

	virtual void mouseReleaseEvent(QMouseEvent* e) override {
		is_mouse_pressed_ = false;

		if (lastTimeClick.elapsed() < QApplication::doubleClickInterval()) {
			slot_maximize_or_restore();
		}

		lastTimeClick.start();
	}

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
};

inline void PageTitle::slot_minimize()
{
	parentWidget()->showMinimized();
}

inline void PageTitle::slot_maximize_or_restore()
{
	if (is_maximized_) {
		maximize_->set_icon_path("Skin/system/max");
		parentWidget()->showNormal();
		is_maximized_ = false;
	} else {
		maximize_->set_icon_path("Skin/system/restore");
		parentWidget()->showMaximized();
		is_maximized_ = true;
	}

	emit sig_maximized(is_maximized_);
}

inline void PageTitle::slot_close()
{
	emit sig_close();
}

}
}
