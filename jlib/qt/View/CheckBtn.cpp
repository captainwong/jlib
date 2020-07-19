#include "CheckBtn.h"
#include <qevent.h>
#include "../QtUtils.h"
#include "../QtStylesheet.h"

using namespace jlib::qt;

CheckBtn::CheckBtn(QWidget *parent, int tag)
	: QLabel(parent)
	, tag_(tag)
	, font_sz_(12)
{
	setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

CheckBtn::~CheckBtn()
{
}

void CheckBtn::set_font_size(int fsz)
{
	font_sz_ = fsz;
	refresh();
}

void CheckBtn::refresh()
{
	if (pressed_) {
		setStyleSheet(build_style(Qt::darkGray, font_sz_));
	} else if (hover_) {
		setStyleSheet(build_style(Qt::lightGray, font_sz_));
	} else if (checked_) {
		setStyleSheet(build_style(Qt::white, font_sz_));
	} else {
		setStyleSheet(build_style(Qt::darkGray, font_sz_));
	}

	adjustSize();
	update();
}

void CheckBtn::set_check(bool check)
{
	MYQDEBUG << check;
	checked_ = check;
	refresh();
}

void CheckBtn::enterEvent(QEvent * e)
{
	setCursor(QCursor(Qt::PointingHandCursor));
	hover_ = true;
	refresh();
	emit sig_focus_on(tag_, true);
}

void CheckBtn::leaveEvent(QEvent * e)
{
	setCursor(QCursor(Qt::ArrowCursor));
	hover_ = false;
	refresh();
}

void CheckBtn::mousePressEvent(QMouseEvent * e)
{
	if (e->button() == Qt::LeftButton) {
		pressed_ = true;
		refresh();
	}
}

void CheckBtn::mouseReleaseEvent(QMouseEvent * e)
{
	if (pressed_ && e->button() == Qt::LeftButton) {
		pressed_ = false;
		checked_ = !checked_;
		refresh();
		MYQDEBUG << tag_ << checked_;
		emit sig_check(tag_, checked_);
	}
}
