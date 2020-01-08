#pragma once

#include <QLabel>

namespace HBVideoPlatform {
namespace common {

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

	IconBtn(QWidget *parent, QString icon_path, uint32_t state_set = IconStatus::status_default);
	~IconBtn();

	void set_pos(const QPoint& pos);
	void set_pos(int x, int y);
	void set_icon_path(const QString& icon_path);
	void set_enabled(bool enable);
	void set_ing_status(bool is_ing);
	bool is_ing_status() const { return is_ing_status_; }

protected:
	virtual void enterEvent(QEvent*) override;
	virtual void leaveEvent(QEvent*) override;
	virtual void mousePressEvent(QMouseEvent*) override;
	virtual void mouseReleaseEvent(QMouseEvent*) override;

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

	void refresh_icon_status();

};

}
}
