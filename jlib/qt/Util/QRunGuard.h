#pragma once

#include <QObject>
#include <QString>
#include <QSharedMemory>
#include <QSystemSemaphore>

#include "../qt_global.h"

JLIBQT_NAMESPACE_BEGIN

class RunGuard
{

public:
	RunGuard(const QString& key);
	~RunGuard();

	bool isAnotherRunning();
	bool tryToRun();
	void release();

private:
	const QString key;
	const QString memLockKey;
	const QString sharedmemKey;

	QSharedMemory sharedMem;
	QSystemSemaphore memLock;

	Q_DISABLE_COPY(RunGuard)
};

JLIBQT_NAMESPACE_END
