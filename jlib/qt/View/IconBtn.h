#pragma once

#include <QLabel>
#include <unordered_map>

class IconBtn : public QLabel
{
	Q_OBJECT

public:
	enum class IconStatus : uint32_t {
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

public:
	IconBtn(QWidget* parent = nullptr, QString iconPath = "", int tag = -1);
	~IconBtn();

	void set_icon(IconStatus status, QString icon_path);
	void set_icon(QString icon_path);

	void set_tag(int tag) { tag_ = tag; }
	int tag() const { return tag_; }

	void set_data(int data) { data_ = data; }
	int data() const { return data_; }

	void set_highlight(bool on = true);
	void set_size(QSize sz) { sz_ = sz; setFixedSize(sz); refresh(); }


	void set_pos(const QPoint& pos){ move(pos); }
	void set_pos(int x, int y){ move(x, y); }
	void set_enabled(bool enabled) { is_enabled_ = enabled; refresh(); }
	void set_ing(bool is_ing) { is_ing_ = is_ing; refresh(); }
	bool is_ing() const { return is_ing_; }

protected:
	virtual void enterEvent(QEvent*) override;
	virtual void leaveEvent(QEvent*) override;
	virtual void mousePressEvent(QMouseEvent*) override;
	virtual void mouseReleaseEvent(QMouseEvent*) override;

	void refresh();

signals:
	void clicked();
	void sig_focus_on(int tag, bool on);
	void sig_clicked(int tag);
	void sig_clicked_with_data(int tag, int data);
	void sig_mouse_enter();
	void sig_mouse_leave();

protected:
	std::unordered_map<IconStatus, QString> icon_path_ = {};
	bool is_enabled_ = true;
	bool is_hover_ = false;
	bool is_press_ = false;
	bool is_ing_ = false;
	bool is_highlight_ = false;
	int tag_ = -1;
	int data_ = 0;
	QSize sz_ = {};
};
