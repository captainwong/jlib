#pragma once

#include "../qt_global.h"
#include <QThread>
#include "../Model/ThreadModel.h"

namespace jlib
{
namespace qt
{

class ThreadCtrl : public QThread
{
	Q_OBJECT

public:
	ThreadCtrl(QObject* parent, int proto_type = -1);
	~ThreadCtrl();

	void setWorker(ThreadWorker worker) { worker_ = worker; }

	void setParam(void* input = nullptr, void* output = nullptr) {
		input_ = input;
		output_ = output;
	}

	void setTag(int tag) { tag_ = tag; }
	int getTag() const { return tag_; }

protected:
	virtual void run() override;

signals:
	void sig_progress(int tag, ThreadProgress progress);
	void sig_done(int tag, int result_code);

private:
	ThreadWorker worker_ = {};
	int proto_type_ = -1;
	int result_code_ = -1;
	void* input_ = nullptr;
	void* output_ = nullptr;

	int tag_ = -1;
};

}
}
