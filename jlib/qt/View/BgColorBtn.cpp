#include "BgColorBtn.h"
#include <jlib/qt/QtStylesheet.h>

namespace HBVideoPlatform {
namespace common {

BgColorBtn::BgColorBtn(QWidget *parent)
	: QPushButton(parent)
{
	connect(this, SIGNAL(clicked()), this, SLOT(slot_clicked()));
}

BgColorBtn::~BgColorBtn()
{
}

void BgColorBtn::set_btn_attr(QColor bg_normal, QColor bg_suspend, QColor font_color, unsigned int font_sz)
{
	bg_normal_ = bg_normal;
	bg_suspend_ = bg_suspend;
	font_color_ = font_color;
	font_sz_ = font_sz;

	set_normal_attr();
}

void BgColorBtn::enterEvent(QEvent * e)
{
	if (!isEnabled() || (parent() && !parentWidget()->isEnabled()))return;
	set_mouse_enter_attr();
	setCursor(QCursor(Qt::PointingHandCursor));
}

void BgColorBtn::leaveEvent(QEvent * e)
{
	if (!isEnabled() || (parent() && !parentWidget()->isEnabled()))return;
	set_normal_attr();
	setCursor(QCursor(Qt::ArrowCursor));
}

void BgColorBtn::set_normal_attr()
{
	set_attr(bg_normal_, font_color_, font_sz_);
}

void BgColorBtn::set_mouse_enter_attr()
{
	set_attr(bg_suspend_, font_color_, font_sz_);
}

void BgColorBtn::set_attr(QColor bg_color, QColor font_color, unsigned int font_sz)
{
	setStyleSheet(jlib::qt::build_style(bg_color, font_color, font_sz));
}

void BgColorBtn::slot_clicked()
{
	if (tag_ != -1) {
		emit sig_clicked(tag_);
	} 
}

}
}
