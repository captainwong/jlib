#pragma once

#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QBitmap>
#include "../QtUtils.h"

namespace jlib
{
namespace qt
{

class IconBtn : public QLabel
{
	Q_OBJECT

public:
	enum IconStatus : uint32_t {
		status_normal = 0x00000001,
		status_hover = 0x00000002,
		status_press = 0x00000004,
		status_disable = 0x00000008,
		status_ing = 0x00000010,

		// 不规则按钮
		type_mask = 0x00000020,
		// 不重绘已绘制区域（原demo建议尽量不用）
		type_opaque_paint = 0x00000040,
		// 长按功能
		type_long_press = 0x00000080,

		status_default = status_normal | status_hover | status_press,
	};

	IconBtn(QWidget *parent, QString icon_path, uint32_t state_set = IconStatus::status_default)
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

	void set_pos(const QPoint& pos) { move(pos); }
	void set_pos(int x, int y) { set_pos(QPoint(x, y)); }

	void set_icon_path(const QString& icon_path) {
		icon_path_ = icon_path;
		refresh_icon_status();
	}

	void set_enabled(bool enable) {
		is_enable_ = enable;
		refresh_icon_status();
	}

	void set_ing_status(bool is_ing) {
		is_ing_status_ = is_ing;
		refresh_icon_status();
	}

	bool is_ing_status() const { return is_ing_status_; }

protected:
	virtual void enterEvent(QEvent*) override {
		if (!is_enable_) { return; }

		setCursor(QCursor(Qt::PointingHandCursor));
		is_mouse_hover_ = true;
		refresh_icon_status();

		emit sig_mouse_enter();
	}

	virtual void leaveEvent(QEvent*) override {
		setCursor(QCursor(Qt::ArrowCursor));
		is_mouse_hover_ = false;
		is_mouse_press_ = false;
		refresh_icon_status();

		emit sig_mouse_leave();
	}

	virtual void mousePressEvent(QMouseEvent*) override {
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

	virtual void mouseReleaseEvent(QMouseEvent* e) override {
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

signals:
	void clicked();
	void long_press_trigger(bool is_press);
	void sig_mouse_enter();
	void sig_mouse_leave();

private slots:
	void slot_long_press_timeout();

private:
	unsigned long state_set_ = 0;
	QString icon_path_ = {};
	bool is_enable_ = true;
	bool is_ing_status_ = false;
	bool is_mouse_hover_ = false;
	bool is_mouse_press_ = false;
	QTimer* long_press_timer_ = {};
	bool is_long_press_ = false;

	void refresh_icon_status() {
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


};

inline void IconBtn::slot_long_press_timeout()
{
	is_long_press_ = true;
	emit long_press_trigger(true);
}

}
}
