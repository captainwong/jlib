#pragma once

#include <QPushButton>
#include "../QtStylesheet.h"

namespace jlib
{
namespace qt
{

class BgColorBtn : public QPushButton
{
	Q_OBJECT

public:
	BgColorBtn(QWidget *parent = nullptr)
		: QPushButton(parent)
	{
		connect(this, &QPushButton::clicked, this, &BgColorBtn::slot_clicked);
	}

	virtual ~BgColorBtn() {}

	void set_btn_attr(QColor bg_normal, QColor bg_suspend, QColor font_color, unsigned int font_sz) {
		bg_normal_ = bg_normal;
		bg_suspend_ = bg_suspend;
		font_color_ = font_color;
		font_sz_ = font_sz;
		set_normal_attr();
	}

	void set_tag(int tag) { tag_ = tag; }
	int tag() const { return tag_; }

signals:
	void sig_clicked(int tag);

private slots:
	void slot_clicked();

protected:
	virtual void enterEvent(QEvent* e) override {
		if (!isEnabled() || (parent() && !parentWidget()->isEnabled()))return;
		set_mouse_enter_attr(); setCursor(QCursor(Qt::PointingHandCursor));
	}

	virtual void leaveEvent(QEvent* e) override {
		if (!isEnabled() || (parent() && !parentWidget()->isEnabled()))return;
		set_normal_attr(); setCursor(QCursor(Qt::ArrowCursor));
	}

private:
	void set_normal_attr() { set_attr(bg_normal_, font_color_, font_sz_); }
	void set_mouse_enter_attr() { set_attr(bg_suspend_, font_color_, font_sz_); }
	void set_attr(QColor bg_color, QColor font_color, unsigned int font_sz) {
		setStyleSheet(build_style(bg_color, font_color, font_sz));
	}

private:
	QColor bg_normal_ = {};
	QColor bg_suspend_ = {};
	QColor font_color_ = {};
	unsigned int font_sz_ = {};
	int tag_ = -1;
};

inline void BgColorBtn::slot_clicked()
{
	if (tag_ != -1) {
		emit sig_clicked(tag_);
	}
}
	
} // namespace qt
} // namespace jlib
