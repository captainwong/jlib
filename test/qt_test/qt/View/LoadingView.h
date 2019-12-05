#pragma once

#include <QDialog>
#include <qlabel.h>
#include "../Ctrl/ThreadCtrl.h"
#include "../Model/ThreadModel.h"
#include <vector>
#include <qelapsedtimer.h>
#include <qmovie.h>
#include "../QtUtils.h"
#include "../QtStylesheet.h"
#include "../QtPathHelper.h"

namespace jlib
{
namespace qt
{

class LoadingView : public QDialog
{
	Q_OBJECT

public:
	enum LoadingViewSize {
		sz_big,
		sz_small,
	};

	LoadingView(QWidget *parent = nullptr, std::vector<ThreadCtrl*> threads = {}, LoadingViewSize sz = sz_big)
		: QDialog(parent)
		, threads_(threads)
		, sz_(sz)
	{
		setWindowModality(Qt::WindowModal);
		setWindowFlags(Qt::FramelessWindowHint);
		if (!parent) { setAttribute(Qt::WA_TranslucentBackground); }

		if (sz == sz_small) { setFixedSize(200, 200);
		} else { setFixedSize(630, 637); }

		label_ = new QLabel(this); label_->resize(width(), height()); label_->move(0, 0);

		progress1_ = new QLabel(this); progress1_->resize(180, 80);
		progress2_ = new QLabel(this); progress2_->resize(180, 50);
		elapse_ = new QLabel(this); elapse_->resize(180, 50);

		progress1_->move((width() - progress1_->width()) / 2, (height() - progress1_->height() - progress2_->height() - elapse_->height()) / 2);
		progress1_->setStyleSheet(build_style(Qt::green, 64));
		progress1_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		progress1_->hide();

		progress2_->move(progress1_->x(), progress1_->y() + progress1_->height());
		progress2_->setStyleSheet(build_style(Qt::blue, 32));
		progress2_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		progress2_->hide();

		elapse_->move(progress1_->x(), progress2_->y() + progress2_->height());
		elapse_->setStyleSheet(build_style(Qt::darkYellow, 48));
		elapse_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		elapse_->hide();

		parent ? center_to_parent(this, parent) : center_to_desktop(this);

		int tag = 0;
		for (auto& thread_ : threads_) {
			thread_->set_tag(tag++);
			tp_.push_back(ThreadProgress(0, 1));
			connect(thread_, SIGNAL(sig_ready(int, std::error_code)), this, SLOT(slot_ready(int, std::error_code)));
			connect(thread_, SIGNAL(sig_progress(int, HBVideoPlatform::common::ThreadProgress)), this, SLOT(slot_progress(int, HBVideoPlatform::common::ThreadProgress)));
		}
	}

	virtual ~LoadingView() {
		for (auto thread_ : threads_) {
			thread_->deleteLater();
		}
	}

	void run() {
		auto path = PathHelper::program();;
		path += sz_ == sz_small ? "/Skin/gif/ajax-loader-small.gif" : "/Skin/gif/preloader.gif";
		auto movie = new QMovie(path);
		label_->setMovie(movie);
		movie->start();

		timer_.start();

		for (auto thread_ : threads_) {
			thread_->start();
		}

		auto p = parentWidget();
		if (p) {
			p->setEnabled(false);
		}

		if (!show_progress_) {
			show_progress_ = true;
			startTimer(1000);
		}

		QDialog::exec();
		if (p) {
			p->setEnabled(true);
		}
	}

	std::error_code get_result() const { return result_; }

private slots:
	void slot_ready(int tag, std::error_code ec);
	void slot_progress(int tag, jlib::qt::ThreadProgress progress);

protected:
	virtual void timerEvent(QTimerEvent* e) override {
		if (show_progress_) {
			int secs = timer_.elapsed() / 1000;
			int mins = secs / 60;
			secs %= 60;
			elapse_->setText(QString().sprintf("%02d:%02d", mins, secs));
			elapse_->show();
		}
	}

private:
	std::error_code result_ = {};
	std::vector<ThreadCtrl*> threads_ = {};

	QLabel* label_ = {};
	QLabel* progress1_ = {};
	QLabel* progress2_ = {};
	QLabel* elapse_ = {};
	std::vector<ThreadProgress> tp_ = {};
	bool show_progress_ = false;
	LoadingViewSize sz_ = sz_big;

	QElapsedTimer timer_ = {};
};

inline void LoadingView::slot_progress(int tag, jlib::qt::ThreadProgress progress)
{
	tp_[tag].total = progress.total;
	tp_[tag].pos = progress.pos;
	int pos = 0; int total = 0;
	for (auto tp : tp_) {
		pos += tp.pos;
		total += tp.total;
	}

	int percent = pos * 100 / total;
	MYQDEBUG << "tag" << tag << "progress.pos" << progress.pos << "progress.total" << progress.total << "pos" << pos << "total" << total << "percent" << percent;

	progress1_->setText(QString("%1%").arg(percent));
	progress1_->show();

	progress2_->setText(QString("%1/%2").arg(pos).arg(total));
	progress2_->show();

	int secs = timer_.elapsed() / 1000;
	int mins = secs / 60;
	secs %= 60;
	elapse_->setText(QString().sprintf("%02d:%02d", mins, secs));
	elapse_->show();


}

inline void LoadingView::slot_ready(int tag, std::error_code result)
{
	MYQDEBUG << "tag" << tag << "ready, code=" << result.value() << "msg=" << QString::fromLocal8Bit(result.message().data());
	result_ = result;

	if (show_progress_) {
		progress1_->setText("100%");
		progress1_->show();
	}

	for (auto iter = threads_.begin(); iter != threads_.end(); iter++) {
		auto thread = *iter;
		if (thread->get_tag() == tag) {
			threads_.erase(iter);
			break;
		}
	}

	if (!threads_.empty()) { return; }

	QDialog::accept();
}



}
}
