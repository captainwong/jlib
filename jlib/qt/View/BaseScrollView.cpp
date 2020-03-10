#include "BaseScrollView.h"
#include <qscrollbar.h>
#include "../QtUtils.h"
#include "../QtStylesheet.h"

using namespace jlib::qt;

BaseScrollView::BaseScrollView(QWidget *parent)
	: QScrollArea(parent)
{
	setFrameShape(QFrame::NoFrame);
	verticalScrollBar()->setStyleSheet(def_style_sheets::vertical_scroll_bar);
	//fill_bg_with_color(this, def_colors::window_bk);

	root_ = new QWidget(this);
	setWidget(root_);

	refresh();
}

BaseScrollView::~BaseScrollView()
{
}

void BaseScrollView::resizeEvent(QResizeEvent *)
{
	root_->resize(width() - 8, qMax(height(), root_->height()));
}

void BaseScrollView::refresh()
{
	root_->move(0, 0);
	root_->resize(width() - 8, qMax(height(), root_->height()));
	//fill_bg_with_color(root_, def_colors::control_bk);
}
