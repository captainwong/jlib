#pragma once

#include "../qt_global.h"
//#include <system_error>
#include <QString>
#include <functional>


typedef std::function<int(void)> ThreadWorker;

struct ThreadProgress {
	int pos = 0;
	int total = 0;

	ThreadProgress() {}
	ThreadProgress(int p, int t) :pos(p), total(t) {}
};

typedef std::function<void(ThreadProgress)> ThreadProgressCB;


//Q_DECLARE_METATYPE(std::error_code)
Q_DECLARE_METATYPE(ThreadProgress)
