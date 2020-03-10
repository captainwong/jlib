#pragma once

#include <QDialog>
#include <qlabel.h>
#include "../Ctrl/ThreadCtrl.h"
#include "../Model/ThreadModel.h"
#include <vector>
#include <qelapsedtimer.h>

//JLIBQT_NAMESPACE_BEGIN

class LoadingView : public QDialog
{
	Q_OBJECT

public:
	enum LoadingViewSize {
		sz_big,
		sz_small,
	};

	LoadingView(QWidget *parent = nullptr, std::vector<ThreadCtrl*> threads = {}, LoadingViewSize sz = sz_big);
	~LoadingView();

	void run();
	std::error_code get_result() const { return result_; }

private slots:
	void slot_ready(int tag, std::error_code ec);
	void slot_progress(int tag, ThreadProgress progress);

protected:
	virtual void timerEvent(QTimerEvent* e) override;

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

//JLIBQT_NAMESPACE_END
