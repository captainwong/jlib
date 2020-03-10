#pragma once

#include <QtWidgets>
#include <vector>
#include <list>
#include <unordered_map>

class CheckBtn;
class PageView : public QWidget
{
	Q_OBJECT

public:
	PageView(QWidget *parent, int max_visible_pages = 20);
	~PageView();

	void set_pages(int pages) { pages_ = pages; page_ = 0; refresh(); }
	void refresh();
	int get_total_page() const { return pages_; }

	void focus_head();
	void focus_left();
	void focus_right();
	void click_current();
	
	//! 导航到某页
	void navigate_page(int page);

protected:
	void refresh_layout();

signals:
	void sig_page_changed(int page);
	void sig_focus_on();
	
private slots:
	void slot_page_checked(int tag, bool is_check);
	void slot_navigator_check(int tag, bool is_check);
	void slot_focus_on(int,bool);

private:
	//! 页码总数
	int pages_ = 0;
	//! 当前页码
	int page_ = 0;
	
	CheckBtn* btn_left_ = {};
	CheckBtn* btn_right_ = {};
	QLabel* info_ = {};

	CheckBtn* cur_check_ = {};

	QHBoxLayout* hbox_ = {};

	//! 所有页码按钮
	std::unordered_map<int, CheckBtn*> page_map_ = {};

	//! 可视页码
	std::list<int> visible_pages_ = {};

	//! 当前高亮按钮
	int currentFocusedId = db_left;

	enum DirectionButton {
		db_left = -1,
		db_right = -2,
	};

	
	int max_visible_pages_ = 20;
	
};
