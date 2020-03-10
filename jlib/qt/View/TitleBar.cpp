#include "TitleBar.h"
#include <QLayout>
#include <QPixmap>
#include <QApplication>
#include "../QtStylesheet.h"

using namespace jlib::qt;

TitleBar::TitleBar(QWidget* parent,
					 QString minIcon,
					 QString maxIcon,
					 QString restoreIcon,
					 QString closeIcon,
					 QString logoIcon)
	: QWidget(parent)
	, minIcon_(minIcon)
	, maxIcon_(maxIcon)
	, restoreIcon_(restoreIcon)
	, closeIcon_(closeIcon)
	, logoIcon_(logoIcon)
{
	setAutoFillBackground(true);

	QPalette pal = palette();
	pal.setColor(QPalette::Window, Qt::black);
	setPalette(pal);

	minimize_ = new IconBtn(this, minIcon); 
	maximize_ = new IconBtn(this, maxIcon); 
	close_ = new IconBtn(this, closeIcon);

	connect(minimize_, SIGNAL(clicked()), this, SLOT(slot_minimize()));
	connect(maximize_, SIGNAL(clicked()), this, SLOT(slot_maximize_or_restore()));
	connect(close_, SIGNAL(clicked()), this, SLOT(slot_close()));

	logo_ = new QLabel(this);
	QPixmap pixmap;
	pixmap.load(logoIcon);
	logo_->setFixedSize(32, 32);
	logo_->setPixmap(pixmap.scaled(32, 32, Qt::KeepAspectRatio));

	title_ = new QLabel(this);
	//title_->setStyleSheet(def_style_sheets::title);
	title_->setStyleSheet(build_style(def_colors::font_normal3, 18));

	layout_ = new QHBoxLayout(this);
	layout_->addWidget(logo_);
	layout_->addSpacing(5);
	layout_->addWidget(title_);
	layout_->addStretch();
	layout_->addWidget(minimize_);
	layout_->addWidget(maximize_);
	layout_->addWidget(close_);
	layout_->setSpacing(0);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setMinimumHeight(7);

	lastTimeClick.start();
}

TitleBar::~TitleBar()
{
}

void TitleBar::set_title(QString title)
{
	title_->setText(title);
}

void TitleBar::set_maximize_btn_visible(bool visible)
{
	visible ? maximize_->show() : maximize_->hide();
}

void TitleBar::set_maximized(bool isMax)
{
	is_maximized_ = isMax;
	if (isMax) {
		maximize_->set_icon(restoreIcon_);
		parentWidget()->showMaximized();
	} else {
		maximize_->set_icon(maxIcon_);
		parentWidget()->showNormal();
	}
}

void TitleBar::mousePressEvent(QMouseEvent* e)
{
	if (!rect().contains(e->pos()))return;
	is_mouse_pressed_ = true;
	mouse_pressed_pos_ = mapToParent(e->pos());
}

void TitleBar::mouseMoveEvent(QMouseEvent* e)
{
	if (!is_mouse_pressed_)return;
	if (is_maximized_) {
		maximize_->set_icon(maxIcon_);
		parentWidget()->showNormal();
		is_maximized_ = false;
	}
	parentWidget()->move(e->globalPos() - mouse_pressed_pos_);
}

void TitleBar::mouseReleaseEvent(QMouseEvent* e)
{
	is_mouse_pressed_ = false;

	if (lastTimeClick.elapsed() < QApplication::doubleClickInterval()) {
		slot_maximize_or_restore();
	}

	lastTimeClick.start();
}

void TitleBar::slot_minimize()
{
	parentWidget()->showMinimized();
}

void TitleBar::slot_maximize_or_restore()
{
	if (is_maximized_) {
		maximize_->set_icon(maxIcon_);
		parentWidget()->showNormal();
		is_maximized_ = false;
	} else {
		maximize_->set_icon(restoreIcon_);
		parentWidget()->showMaximized();
		is_maximized_ = true;
	}

	emit sig_maximized(is_maximized_);
}

void TitleBar::slot_close()
{
	emit sig_close();
}
