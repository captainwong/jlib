#pragma once

#include <QtWidgets>
#include <vector>
#include "BaseScrollView.h"

class CheckBtn;
class TextMenu : public BaseScrollView
{
	Q_OBJECT

public:
	TextMenu(QWidget *parent, int font_sz);
	~TextMenu();

	void add_menu(int tag, QString text);
	void refresh(QPoint pt);

	// return true for success, false for back
	bool focus_down();
	// return true for success, false for back
	bool focus_up();

	void click_focus();

protected:
	virtual void focusOutEvent(QFocusEvent* e) override;

signals:
	void sig_menu_check(int tag);
	void sig_menu_dead();

private slots:
	void slot_check(int tag, bool is_check);

private:
	int sel_ = -1;
	int focus_ = -1;
	std::vector<CheckBtn*> menu_items_ = {};
	int font_sz_ = 18;
};
