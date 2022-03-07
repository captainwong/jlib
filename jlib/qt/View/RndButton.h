#pragma once

#include <QtWidgets>

//namespace HBVideoPlatform {
//namespace common {

class RndButton : public QWidget
{
	Q_OBJECT

public:
	RndButton(QWidget* parent = nullptr);
	virtual ~RndButton();

	void set_attr(const QString& txt, const QSize& btn_sz = { 60, 30 }, int font_size = 12);

	void set_text(const QString& txt);
	void set_size(const QSize& sz = { 60, 30 });
	void set_font_size(int font_size = 12);
	void set_highlight(bool on = true);
	void set_normal_color(QColor color = 0x2c2d30);
	void set_highlight_color(QColor color = Qt::gray);
	void set_text_color(QColor color = Qt::white);

protected:
	virtual void paintEvent(QPaintEvent* e) override;
	virtual void enterEvent(QEvent* e) override;
	virtual void leaveEvent(QEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;

signals:
	void clicked();
	void sig_focus_on();

private:
	QLabel* txt_ = {};
	int font_sz_ = {};
	QColor bk_color_ = {};
	QColor normal_color_ = {};
	QColor hightlight_color_ = {};
	QColor txt_color_ = {};
	bool is_pressed_ = false;
	bool is_highlighted_ = false;
};


//}
//}
