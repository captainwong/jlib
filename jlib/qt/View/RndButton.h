#pragma once

#include <QtWidgets>

namespace HBVideoPlatform {
namespace common {

class RndButton : public QWidget
{
	Q_OBJECT

public:
	RndButton(QWidget *parent);
	~RndButton();

	void set_attr(QString txt, QSize btn_sz = { 60, 30 }, int font_size = 14);

	void set_highlight(bool on = true);

protected:
	virtual void paintEvent(QPaintEvent* e) override;
	virtual void enterEvent(QEvent* e) override;
	virtual void leaveEvent(QEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;

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
