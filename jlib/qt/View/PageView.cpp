#include "PageView.h"
#include <qlayout.h>
#include "CheckBtn.h"
#include "../QtUtils.h"
#include "../QtStyleSheet.h"

using namespace jlib::qt;

static const int PAGE_FONT_SIZE = 24;

static void center_cursor(QWidget* w) {
	auto pt = w->mapToGlobal(w->rect().center());
	SetCursorPos(pt.x(), pt.y());
}


PageView::PageView(QWidget *parent, int max_visible_pages)
	: QWidget(parent)
	, max_visible_pages_(max_visible_pages)
{
	setFixedHeight(30);

	btn_left_ = new CheckBtn(this, db_left);
	btn_left_->set_font_size(PAGE_FONT_SIZE);
	btn_left_->setText("<");
	connect(btn_left_, SIGNAL(sig_check(int, bool)), this, SLOT(slot_navigator_check(int, bool)));
	connect(btn_left_, SIGNAL(sig_focus_on(int, bool)), this, SLOT(slot_focus_on(int, bool)));

	btn_right_ = new CheckBtn(this, db_right);
	btn_right_->set_font_size(PAGE_FONT_SIZE);
	btn_right_->setText(">");
	connect(btn_right_, SIGNAL(sig_check(int, bool)), this, SLOT(slot_navigator_check(int, bool)));
	connect(btn_right_, SIGNAL(sig_focus_on(int, bool)), this, SLOT(slot_focus_on(int, bool)));

	info_ = new QLabel(this);
	info_->setStyleSheet(build_style(Qt::darkGray, PAGE_FONT_SIZE));
}

PageView::~PageView()
{
}

void PageView::refresh()
{
	for (auto i : page_map_) {
		i.second->deleteLater();
	}

	page_map_.clear();
	visible_pages_.clear();

	for (int i = 0; i < pages_; i++) {
		auto chk = new CheckBtn(this, i);
		chk->set_font_size(PAGE_FONT_SIZE);
		chk->setText(QString::number(i + 1));
		connect(chk, SIGNAL(sig_check(int, bool)), this, SLOT(slot_page_checked(int, bool)));
		connect(chk, SIGNAL(sig_focus_on(int, bool)), this, SLOT(slot_focus_on(int, bool)));
		chk->set_check(false);
		chk->hide();

		if (i < max_visible_pages_) {
			visible_pages_.push_back(i);
		}
		page_map_[i] = chk;
	}

	if (pages_ > 0) {
		cur_check_ = page_map_[0];
		cur_check_->set_check(true);
	}

	refresh_layout();
}

void PageView::focus_head()
{
	center_cursor(btn_left_);
}

void PageView::focus_left()
{
	if (currentFocusedId == db_left) {
		// nothing to do
	} else if (currentFocusedId == db_right) {
		if (visible_pages_.empty()) {
			focus_head();
		} else {
			center_cursor(page_map_[visible_pages_.back()]);
		}
	} else {
		if (currentFocusedId == visible_pages_.front()) {
			focus_head();
		} else {
			currentFocusedId--;
			center_cursor(page_map_[currentFocusedId]);
		}
	}

}

void PageView::focus_right()
{
	if (currentFocusedId == db_left) {
		if (visible_pages_.empty()) {
			center_cursor(btn_right_);
		} else {
			center_cursor(page_map_[visible_pages_.front()]);
		}
	} else if (currentFocusedId == db_right) {
		// nothing to do
	} else {
		if (currentFocusedId == visible_pages_.back()) {
			center_cursor(btn_right_);
		} else {
			currentFocusedId++;
			center_cursor(page_map_[currentFocusedId]);
		}
	}
}

void PageView::click_current()
{
	slot_navigator_check(currentFocusedId, true);
}

void PageView::navigate_page(int page)
{
	if (page < 0 || page >= pages_) { return; }
	
	if (page < visible_pages_.front()) {
		// 在左边不可见
		for (int p = visible_pages_.front() - 1; p >= page; p--) {
			visible_pages_.push_front(p);
			page_map_[visible_pages_.back()]->hide();
			visible_pages_.pop_back();
		}
		slot_page_checked(page, true);
		refresh_layout();
	} else if (page > visible_pages_.back()) {
		// 在右边不可见
		for (int p = visible_pages_.back() + 1; p <= page; p++) {
			visible_pages_.push_back(p);
			page_map_[visible_pages_.front()]->hide();
			visible_pages_.pop_front();
		}
		slot_page_checked(page, true);
		refresh_layout();
	} else {
		// 可见
		slot_page_checked(page, true);
	}

}

void PageView::refresh_layout()
{
	btn_left_->set_check(false);
	btn_right_->set_check(false);
	info_->setText(QString::fromLocal8Bit("共 %1 页").arg(pages_));

	if (hbox_) {
		delete hbox_;
	}

	hbox_ = new QHBoxLayout();
	hbox_->setMargin(0);
	hbox_->setSpacing(20);
	hbox_->addStretch();
	hbox_->addWidget(btn_left_);
	
	for (auto i : visible_pages_) {
		auto chk = page_map_[i];
		chk->show();
		if (i == page_) {
			chk->set_check();
		}

		hbox_->addWidget(chk);
	}

	hbox_->addWidget(btn_right_);
	hbox_->addWidget(info_);
	hbox_->addStretch();

	setLayout(hbox_);
}

void PageView::slot_page_checked(int tag, bool is_check)
{
	if (!is_check) { page_map_[tag]->set_check(true); return; }
	page_ = tag;

	if (cur_check_) {
		cur_check_->set_check(false);
	}
	cur_check_ = page_map_[page_];

	// 2018-2-7 23:49:04 为了APP控制时高亮
	cur_check_->set_check(true);

	emit sig_page_changed(page_);
}

void PageView::slot_navigator_check(int tag, bool is_check)
{
	if (tag == db_left) {
		btn_left_->set_check(false);
		if (--page_ < 0) {
			page_ = 0;
			return;
		}

		if (visible_pages_.front() > page_) {
			visible_pages_.push_front(page_);
			page_map_[visible_pages_.back()]->hide();
			visible_pages_.pop_back();
		}
	} else if (tag == db_right) {
		btn_right_->set_check(false);
		if (++page_ == pages_) {
			page_ = pages_ - 1;
			return;
		}

		if (visible_pages_.back() < page_) {
			visible_pages_.push_back(page_);
			page_map_[visible_pages_.front()]->hide();
			visible_pages_.pop_front();
		}
	} 

	slot_page_checked(page_, true);
	refresh_layout();
}

void PageView::slot_focus_on(int tag, bool)
{
	currentFocusedId = tag;
	emit sig_focus_on();
}
