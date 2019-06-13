#pragma once
#include <QDir>
#include <QCoreApplication>
#include <QString>
#include <QDateTime>

namespace jlib {
namespace qt {

static const auto FILE_TIME_FORMAT = "yyyy-MM-dd_hh-mm-ss";

struct PathHelper 
{
	static inline bool mkpath(const QString& path) {
		QDir dir;
		return dir.exists(path) ? true : dir.mkpath(path);
	}

	static inline QString program() {
		struct Helper {
			Helper() {
				QDir dir(QCoreApplication::applicationDirPath());
				dir.cdUp();
				path = dir.absolutePath();
			}

			QString path = {};
		};
		
		static Helper helper;
		return helper.path;
	}

	static inline QString bin() {
		return program() + "/bin";
	}
	
	static inline QString log() {
		return program() + "/log";
	}

	static inline QString sdk() {
		return program() + "/sdks";
	}

	static inline QString data() {
		return program() + "/data";
	}

	static inline QString config() {
		return data() + "/config";
	}

	static inline QString db() {
		return data() + "/db";
	}

	static inline QString pic() {
		return data() + "/pic";
	}

	static inline QString rec() {
		return data() + "/rec";
	}

	static inline QString currentTimeString() {
		return QDateTime::currentDateTime().toString(FILE_TIME_FORMAT);
	}
};

struct AutoSwithToBin {
	AutoSwithToBin() {}

	AutoSwithToBin(const QString& path) {
		QDir().setCurrent(path);
	}

	~AutoSwithToBin() {
		QDir().setCurrent(PathHelper::bin());
	}
};

} // end of namespace qt
} // end of namespace jlib
