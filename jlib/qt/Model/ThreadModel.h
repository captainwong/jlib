#pragma once

#include <QString>
#include <functional>

namespace jlib
{
namespace qt
{


typedef std::function<std::error_code(void)> ThreadWorker;

struct ThreadProgress {
	int pos = 0;
	int total = 0;

	ThreadProgress() {}
	ThreadProgress(int p, int t) :pos(p), total(t) {}
};

typedef std::function<void(ThreadProgress)> ThreadProgressCB;

}
}

Q_DECLARE_METATYPE(std::error_code)
Q_DECLARE_METATYPE(jlib::qt::ThreadProgress)
