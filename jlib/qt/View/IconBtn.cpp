#include "IconBtn.h"
#include <QPainter>
#include <QStylePainter>
#include <QStyleOption>
#include <QMouseEvent>
#include "../QtUtils.h"
#include "../QtStylesheet.h"

using namespace jlib::qt;

IconBtn::IconBtn(QWidget* parent, QString icon_path, int tag)
	: QLabel(parent)
	, tag_(tag)
{
	setAttribute(Qt::WA_TranslucentBackground, true);
	setStyleSheet(build_style(Qt::white, 18)); 
	if (icon_path.isEmpty()) {
		icon_path_.clear();
	} else {
		icon_path_[IconStatus::status_normal] = icon_path + "_n.png";
		icon_path_[IconStatus::status_hover] = icon_path + "_h.png";
		icon_path_[IconStatus::status_press] = icon_path + "_p.png";
		icon_path_[IconStatus::status_disable] = icon_path + "_d.png";
		icon_path_[IconStatus::status_ing] = icon_path + "_ing.png";
	}	
	refresh();
}

IconBtn::~IconBtn()
{
}

void IconBtn::set_icon(IconStatus status, QString icon_path)
{
	icon_path_[status] = icon_path;
	refresh();
}

void IconBtn::set_icon(QString icon_path)
{
	icon_path_[IconStatus::status_normal] = icon_path + "_n.png";
	icon_path_[IconStatus::status_hover] = icon_path + "_h.png";
	icon_path_[IconStatus::status_press] = icon_path + "_p.png";
	icon_path_[IconStatus::status_disable] = icon_path + "_d.png";
	icon_path_[IconStatus::status_ing] = icon_path + "_ing.png";
	refresh();
}

void IconBtn::set_highlight(bool on)
{
	is_highlight_ = on;
	refresh();
}

void IconBtn::enterEvent(QEvent*)
{
	if (!is_enabled_ || !isEnabled() || (parent() && !parentWidget()->isEnabled()))return;
	setCursor(QCursor(Qt::PointingHandCursor));
	is_hover_ = true;
	setFocus();
	refresh();
	emit sig_focus_on(tag_, true);
	emit sig_mouse_enter();
}

void IconBtn::leaveEvent(QEvent*)
{
	if (!is_enabled_ || !isEnabled() || (parent() && !parentWidget()->isEnabled()))return;
	setCursor(QCursor(Qt::ArrowCursor));
	is_hover_ = false;
	is_press_ = false;
	refresh();
	emit sig_mouse_leave();
}

void IconBtn::mousePressEvent(QMouseEvent*)
{
	if (!is_enabled_ || !isEnabled() || (parent() && !parentWidget()->isEnabled()))return;
	is_press_ = true;
	setFocus();
	refresh();
}

void IconBtn::mouseReleaseEvent(QMouseEvent* e)
{
	if (!is_enabled_ || !isEnabled() || (parent() && !parentWidget()->isEnabled()))return;
	is_press_ = false;
	refresh();

	if (Qt::LeftButton == e->button()) {
		if (rect().contains(e->pos())) {
			MYQDEBUG << "IconBtn emit clicked";
			emit clicked();
			emit sig_clicked(tag_);
			emit sig_clicked_with_data(tag_, data_);
		}
	}
}

void IconBtn::refresh()
{
	QString icon_path;
	if (!is_enabled_ && icon_path_.find(IconStatus::status_disable) != icon_path_.end()) {
		icon_path = icon_path_[IconStatus::status_disable];
	} else if (is_press_ && icon_path_.find(IconStatus::status_press) != icon_path_.end()) {
		icon_path = icon_path_[IconStatus::status_press];
	} else if ((is_hover_ || is_highlight_) && icon_path_.find(IconStatus::status_hover) != icon_path_.end()) {
		icon_path = icon_path_[IconStatus::status_hover];
	} else if (icon_path_.find(IconStatus::status_normal) != icon_path_.end()) {
		icon_path = icon_path_[IconStatus::status_normal];
	}

	if (!icon_path.isEmpty()) {
		QPixmap pixmap;
		LOAD_PIXMAP_EX(icon_path);
		if (sz_.isValid()) {
			resize(sz_);
			QSize pixSize = pixmap.size();
			pixSize.scale(sz_, Qt::IgnoreAspectRatio);
			auto spixmap = pixmap.scaled(pixSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
			setPixmap(spixmap);
		} else {
			resize(pixmap.size());
			setPixmap(pixmap);
		}
	}

	update();
}
