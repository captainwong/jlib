#pragma once

#include <QLabel>

class CheckBtn : public QLabel
{
	Q_OBJECT

public:
	CheckBtn(QWidget *parent, int tag = -1);
	~CheckBtn();

	void set_font_size(int fsz);

	void refresh();
	void set_check(bool check = true);
	bool is_check() const { return checked_; }
	void set_tag(int tag) { tag_ = tag; }
	int tag() const { return tag_; }

protected:
	virtual void enterEvent(QEvent* e) override;
	virtual void leaveEvent(QEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;

signals:
	void sig_check(int tag, bool is_check);
	void sig_focus_on(int tag, bool on);

private:
	bool pressed_ = false;
	bool hover_ = false;
	bool checked_ = false;

	int tag_ = -1;
	int font_sz_ = 18;
};
