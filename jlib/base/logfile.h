#pragma once

#include "config.h"
#include "noncopyable.h"
#include <mutex>
#include <memory>
#include <string>
#include <assert.h>
#include <sys/types.h>  // for off_t

namespace jlib
{

namespace FileUtil
{
class AppendFile;
}

class LogFile : noncopyable
{
public:
	LogFile(const std::string& basename, off_t rollSize, bool threadSafe = true, int flushInterval = 3, int checkEveryN = 1024)
		: basename_(basename)
		, rollSize_(rollSize)
		, flushInterval_(flushInterval)
		, checkEveryN_(checkEveryN)
		, count_(0)
		, mutex_(threadSafe ? std::make_unique<std::mutex>() : nullptr)
		, startOfPeriod_(0)
		, lastRoll_(0)
		, lastFlush_(0)
	{
		assert(basename_.find('/') == std::string::npos);
		assert(basename_.find('\\') == std::string::npos);
		rollFile();
	}

	~LogFile() = default;

	void append(const char* logLine, int len) {
		if (mutex_) {
			std::lock_guard<std::mutex> lg(*mutex_);
			appendUnlocked(logLine, len);
		} else {
			appendUnlocked(logLine, len);
		}
	}

	void flush() {
		if (mutex_) {
			std::lock_guard<std::mutex> lg(*mutex_);
			file_->flush();
		} else {
			file_->flush();
		}
	}

	bool rollFile() {

	}

protected:
	void appendUnlocked(const char* logLine, int len) {
		file_->append(logLine, len);
		
	}

	static std::string getLogFileName(const std::string& basename, time_t now) {

	}

private:
	const std::string basename_;
	const off_t rollSize_;
	const int flushInterval_;
	const int checkEveryN_;

	int count_;
	std::unique_ptr<std::mutex> mutex_;
	time_t startOfPeriod_;
	time_t lastRoll_;
	time_t lastFlush_;
	std::unique_ptr<FileUtil::AppendFile> file_;

	static constexpr int ROLL_PER_SECONDS = 24 * 60 * 60;
};


}
