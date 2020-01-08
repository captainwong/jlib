#include "Toast.h"
#include <jlib/qt/QtStylesheet.h>

namespace HBVideoPlatform {
namespace common {

static Toast* prevToast = nullptr;

void Toast::makeToast(QWidget * context, const QString & msg, ToastLength length)
{
	if (prevToast) {
		prevToast->killTimer(prevToast->timerId);
	} else {
		prevToast = new Toast(context);
	}

	prevToast->length_ = length;
	prevToast->setParent(context);
	prevToast->buildMsg(msg);
	prevToast->show();
	prevToast->timerId = prevToast->startTimer(1000);
}


Toast::Toast(QWidget* parent)
	: QLabel(parent)
{
	//setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);

	//label_ = new QLabel(this);
	//label_->setFont(QFont(def_font_families::yahei, 24));
	//setWordWrap(true);
	setAlignment(Qt::AlignCenter);
	setStyleSheet(jlib::qt::build_style(Qt::black, Qt::white, 12));
}

void Toast::buildMsg(const QString & msg)
{
	auto rc = parentWidget()->rect();
	resize(rc.size());

	auto fm = fontMetrics();
	int w = fm.width(msg) * 2 + 10;
	w = w > rc.width() ? rc.width() : w;
	//int h = fm.height();
	int h = 50;

	resize(w, h);
	move((rc.width() - w) / 2, (rc.height() - h) / 2);

	setText(msg);
}

void Toast::timerEvent(QTimerEvent * e)
{
	if (length_-- == 0) {
		hide();
		deleteLater();
		prevToast = nullptr;
	}
}


}
}
