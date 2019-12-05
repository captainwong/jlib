#pragma once

#include <QThread>
#include "../Model/ThreadModel.h"
#include <thread>

namespace jlib
{
namespace qt
{


class ThreadCtrl : public QThread
{
	Q_OBJECT

public:
	ThreadCtrl(QObject *parent, ThreadWorker worker)
		: QThread(parent)
		, worker_(worker)
	{
	}

	~ThreadCtrl() {}

	void set_worker(ThreadWorker worker) { worker_ = worker; }

	void set_tag(int tag) { tag_ = tag; }
	int get_tag() const { return tag_; }

protected:
	virtual void run() override {
		std::error_code result;

		if (worker_) {
			result = worker_();
		}

		emit sig_ready(tag_, result);
	}

signals:
	void sig_ready(int tag, std::error_code result);
	void sig_progress(int tag, jlib::qt::ThreadProgress progress);

private:
	ThreadWorker worker_ = {};

	void* input_ = nullptr;
	void* output_ = nullptr;

	int tag_ = 0;
};

}
}
