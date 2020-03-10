#include "TextMenu.h"
#include "CheckBtn.h"
#include "../QtUtils.h"
#include "../QtStyleSheet.h"

using namespace jlib::qt;


TextMenu::TextMenu(QWidget *parent, int font_sz)
	: BaseScrollView(parent)
	, font_sz_(font_sz)
{

}

TextMenu::~TextMenu()
{
}

void TextMenu::add_menu(int tag, QString text)
{
	auto chk = new CheckBtn(root(), tag);
	chk->setText(text);
	chk->set_font_size(font_sz_);
	chk->refresh();
	menu_items_.push_back(chk);
	connect(chk, SIGNAL(sig_check(int, bool)), this, SLOT(slot_check(int, bool)));
}

void TextMenu::refresh(QPoint pt)
{
	if (menu_items_.empty())return;

	int gap = 2;
	auto sample_chk = menu_items_.front();
	int max_width = sample_chk->width();
	int max_y = parentWidget()->rect().bottom();
	int y = 0;
	for (auto& chk : menu_items_) {
		chk->move(0, y);
		y += chk->height() + gap;

		if (height() < y && pt.y() + y < max_y) {
			setFixedHeight(y);
		}

		if (root()->height() < y) {
			root()->setFixedHeight(y);
		}

		if (max_width < chk->width()) {
			max_width = chk->width();
			root()->setFixedWidth(max_width);
		}

		MYQDEBUG << "max_width" << max_width;
	}

	setFixedWidth(max_width + 10);
	move(pt.x(), pt.y());
	adjustSize();
	setFocus();
}

bool TextMenu::focus_down()
{
	if (!menu_items_.empty()) {
		if (++focus_ == menu_items_.size()) {
			focus_--;
		}
		MYQDEBUG << "focus_=" << focus_;
		auto iter = menu_items_.begin();
		for (int i = 0; i < focus_; i++) {
			iter++;
		}

		auto item = *iter;
		ensureWidgetVisible(item);

		auto pt = item->mapToGlobal(item->rect().center());
		SetCursorPos(pt.x(), pt.y());
		return true;
	}
	return false;
}

bool TextMenu::focus_up()
{
	if (!menu_items_.empty()) {
		if (--focus_ < 0) {
			focus_ = 0;
		}

		MYQDEBUG << "focus_=" << focus_;

		auto iter = menu_items_.begin();
		for (int i = 0; i < focus_; i++) {
			iter++;
		}

		auto item = *iter;
		ensureWidgetVisible(item);

		auto pt = item->mapToGlobal(item->rect().center());
		SetCursorPos(pt.x(), pt.y());
		return true;
	}
	return false;
}

void TextMenu::click_focus()
{
	auto iter = menu_items_.begin();
	for (int i = 0; i < focus_ && iter != menu_items_.end(); i++) {
		iter++;
	}

	if (iter != menu_items_.end()) {
		auto item = *iter;
		slot_check(item->tag(), true);
	}
}

void TextMenu::focusOutEvent(QFocusEvent * e)
{
	auto top = topLevelWidget();
	if (isAncestorOf(top)) {
		return;
	}
	hide();
	emit sig_menu_dead();
	deleteLater();
}

void TextMenu::slot_check(int tag, bool is_check)
{
	if (!is_check)return;
	MYQDEBUG << tag << is_check;
	emit sig_menu_check(tag);
	parentWidget()->setFocus();
}
