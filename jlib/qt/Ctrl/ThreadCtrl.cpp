#include "ThreadCtrl.h"


JLIB_QT_NAMESPACE_BEGIN

ThreadCtrl::ThreadCtrl(QObject* parent, int proto_type)
	: QThread(parent)
	, proto_type_(proto_type)
{
}

ThreadCtrl::~ThreadCtrl()
{
}

void ThreadCtrl::run()
{
	if (worker_) {
		result_code_ = worker_();
	}

	emit sig_done(tag_, result_code_);
}

JLIB_QT_NAMESPACE_END
