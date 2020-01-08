#include "IconBtn.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QBitMap>
#include <QtGui/QPainter>
#include <QDebug>
#include <QTimer>
#include <jlib/qt/QtUtils.h>

namespace HBVideoPlatform {
namespace common {

IconBtn::IconBtn(QWidget *parent, QString icon_path, uint32_t state_set)
	: QLabel(parent)
	, icon_path_(icon_path)
	, state_set_(state_set)
{
	setAttribute(Qt::WA_TranslucentBackground, true);
	long_press_timer_ = new QTimer(this);
	connect(long_press_timer_, SIGNAL(timeout()), this, SLOT(slot_long_press_timeout()));

	state_set_ |= IconStatus::status_normal;
	set_icon_path(icon_path_);
}

IconBtn::~IconBtn()
{

}

void IconBtn::set_pos(const QPoint & pos)
{
	move(pos);
}

void IconBtn::set_pos(int x, int y)
{
	set_pos(QPoint(x, y));
}

void IconBtn::set_icon_path(const QString & icon_path)
{
	icon_path_ = icon_path;
	refresh_icon_status();
}

void IconBtn::set_enabled(bool enable)
{
	is_enable_ = enable;
	refresh_icon_status();
}

void IconBtn::set_ing_status(bool is_ing)
{
	is_ing_status_ = is_ing;
	refresh_icon_status();
}

void IconBtn::enterEvent(QEvent *)
{
	if (!is_enable_) { return; }

	setCursor(QCursor(Qt::PointingHandCursor));
	is_mouse_hover_ = true;
	refresh_icon_status();

	emit sig_mouse_enter();
}

void IconBtn::leaveEvent(QEvent *)
{
	setCursor(QCursor(Qt::ArrowCursor));
	is_mouse_hover_ = false;
	is_mouse_press_ = false;
	refresh_icon_status();

	emit sig_mouse_leave();
}

void IconBtn::mousePressEvent(QMouseEvent *)
{
	if (!is_enable_) { return; }

	is_mouse_press_ = true;
	refresh_icon_status();

	if (state_set_ & type_long_press) {
		if (long_press_timer_->isActive()) {
			long_press_timer_->stop();
		}

		long_press_timer_->setSingleShot(true);
		long_press_timer_->start(450);
	}
}

void IconBtn::mouseReleaseEvent(QMouseEvent * e)
{
	if (!is_enable_) { return; }

	if (long_press_timer_->isActive()) {
		long_press_timer_->stop();
	}

	bool is_long_press = is_long_press_;
	is_long_press_ = is_mouse_press_ = false;
	refresh_icon_status();

	if (Qt::LeftButton == e->button()) {
		if (rect().contains(e->pos())) {
			is_long_press ? emit long_press_trigger(false) : emit clicked();
		}
	}
}

void IconBtn::refresh_icon_status()
{
	QString icon_path;
	if (!is_enable_) {
		if (!(state_set_ & status_disable)) { return; }
		icon_path = icon_path_ + "_d.png";
	} else if (is_mouse_press_) {
		if (!(state_set_ & status_press)) { return; }
		icon_path = icon_path_ + "_h.png";
	} else if (is_ing_status_) {
		if (!(state_set_ & status_ing)) { return; }
		icon_path = icon_path_ + "_ing.png";
	} else if (is_mouse_hover_) {
		if (!(state_set_ & status_hover)) { return; }
		icon_path = icon_path_ + "_p.png";
	} else {
		if (!(state_set_&status_normal)) { return; }
		icon_path = icon_path_ + "_n.png";
	}

	QPixmap pixmap;
	if (!LOAD_PIXMAP_EX(icon_path)) {
		return;
	}

	setFixedSize(pixmap.size());
	setPixmap(pixmap);

	if (state_set_ & type_mask) {
		setMask(pixmap.mask());
	}

	if (state_set_ & type_opaque_paint) {
		setAttribute(Qt::WA_OpaquePaintEvent, true);
	}
}

void IconBtn::slot_long_press_timeout()
{
	is_long_press_ = true;
	emit long_press_trigger(true);
}

}
}
