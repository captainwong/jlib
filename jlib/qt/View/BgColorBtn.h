#pragma once

#include <QPushButton>

namespace HBVideoPlatform {
namespace common {

class BgColorBtn : public QPushButton
{
	Q_OBJECT

public:
	BgColorBtn(QWidget *parent = nullptr);
	~BgColorBtn();

	void set_btn_attr(QColor bg_normal, QColor bg_suspend, QColor font_color, unsigned int font_sz);

	void set_tag(int tag) { tag_ = tag; }
	int tag() const { return tag_; }

signals:
	void sig_clicked(int tag);

private slots:
	void slot_clicked();

protected:
	virtual void enterEvent(QEvent* e) override;
	virtual void leaveEvent(QEvent* e) override;

private:
	void set_normal_attr();
	void set_mouse_enter_attr();
	void set_attr(QColor bg_color, QColor font_color, unsigned int font_sz);

private:
	QColor bg_normal_ = {};
	QColor bg_suspend_ = {};
	QColor font_color_ = {};
	unsigned int font_sz_ = {};

	int tag_ = -1;
};

}
}
