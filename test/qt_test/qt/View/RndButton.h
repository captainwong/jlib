#pragma once

#include <QtWidgets>
#include "../QtStylesheet.h"

namespace jlib
{
namespace qt
{

class RndButton : public QWidget
{
	Q_OBJECT

public:
	RndButton(QWidget *parent)
		: QWidget(parent)
	{
		txt_ = new QLabel(this);
		txt_->setAlignment(Qt::AlignCenter);
		txt_->hide();
	}

	~RndButton() {}

	void set_attr(QString txt, QSize btn_sz = { 60, 30 }, int font_size = 14) {
		font_sz_ = font_size;
		txt_->setStyleSheet(build_style(Qt::white, font_size));
		txt_->setText(txt);
		setFixedSize(btn_sz);

		/*QPixmap pixmap;
		LOAD_PIXMAP_EX(QString::fromLocal8Bit("Skin/Ó¦ÓÃ¿ò1.png"));
		QSize pixSize = pixmap.size();
		pixSize.scale(sz, Qt::KeepAspectRatio);
		pixmap_ = pixmap.scaled(pixSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);*/

		txt_->resize(size());
		txt_->move(0, 0);
		txt_->show();

		bk_color_ = def_colors::control_bk;

		update();
	}

	void set_highlight(bool on = true) {
		is_highlighted_ = on;
		bk_color_ = is_highlighted_ ? Qt::lightGray : def_colors::control_bk;
		update();
	}

protected:
	virtual void paintEvent(QPaintEvent* e) override {
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing, true);
		QPainterPath path;
		//int radius = std::min(rect().width() / 20, rect().height() / 20);
		path.addRoundedRect(QRectF(0, 0, width(), height()), 10, 10);
		QPen pen(Qt::black, 1);
		painter.setPen(pen);
		painter.fillPath(path, bk_color_);
		painter.drawPath(path);
	}
	virtual void enterEvent(QEvent* e) override {
		setCursor(QCursor(Qt::PointingHandCursor));
		bk_color_ = Qt::darkGray;
		update();
	}

	virtual void leaveEvent(QEvent* e) override {
		setCursor(QCursor(Qt::ArrowCursor));

		bk_color_ = is_highlighted_ ? Qt::lightGray : def_colors::control_bk;
		update();

		is_pressed_ = false;
	}

	virtual void mousePressEvent(QMouseEvent* e) override {
		bk_color_ = def_colors::control_bk;
		update();

		is_pressed_ = true;
	}

	virtual void mouseReleaseEvent(QMouseEvent* e) override {
		bk_color_ = Qt::darkGray;
		update();

		if (is_pressed_) {
			emit clicked();
			is_pressed_ = false;
		}
	}

signals:
	void clicked();

private:
	QLabel* txt_ = {};
	QPixmap pixmap_ = {};
	int font_sz_ = {};
	QColor bk_color_ = {};
	QColor txt_color_ = {};
	bool is_pressed_ = false;
	bool is_highlighted_ = false;
};

}
}
