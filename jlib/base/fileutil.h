#pragma once

#include "config.h"
#include <sys/types.h>  // for off_t
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include <algorithm>
#include "cast.h"
#include "noncopyable.h"
#include "stringpiece.h"


namespace jlib
{
namespace FileUtil
{

//! file size < 64KB
class ReadSmallFile : noncopyable
{
public:
	ReadSmallFile(StringArg filename)
		: fd_(::fopen(filename.c_str(), "r"))
		, err_(0)
	{
		buf_[0] = '\0';
		if (fd_ < 0) {
			err_ = errno;
		}
	}

	~ReadSmallFile() {
		if (fd_ >= 0) {
			::fclose(fd_);
		}
	}

	template <typename StringType>
	int readToString(int maxSize, StringType* content, int64_t* fileSize, int64_t* modifyTime, int64_t* createTime) {
		static_assert(sizeof(off_t) == 8, "sizeof(off_t) != 8");
		assert(content);
		int err = err_;
		if (fd_ >= 0) {
			content->clear();
			if (fileSize) {
				struct stat statbuf;
				if (::fstat(fd_, &statbuf) == 0) {
					if (S_IFREG & (statbuf.st_mode)) {
						*fileSize = statbuf.st_size;
						content->reserve(static_cast<int>(std::min(implicit_cast<int64_t>(maxSize), *fileSize)));
					} else if (S_IFDIR & statbuf.st_mode) {
						err = EISDIR;
					}
					
					if (modifyTime) {
						*modifyTime = statbuf.st_mtime;
					}

					if (createTime) {
						*createTime = statbuf.st_ctime;
					}
				} else {
					err = errno
				}
			}

			while (content->size() < implicit_cast<size_t>(maxSize)) {
				size_t toRead = std::min(implicit_cast<size_t>(maxSize) - content->size(), sizeof(buf_));
				size_t n = ::fread(buf_, 1, toRead, fd_);
				if (n > 0) {
					content->append(buf_, n);
				} else {
					if (n < 0) {
						err = errno;
					}
					break;
				}
			}
		}

		return err;
	}

	int readToBuffer(int* size) {
		int err = err_;
		if (fd_ >= 0) {
			size_t n = ::fread(buf_, 1, sizeof(buf_) - 1, fd_);
			if (n >= 0) {
				if (size) {
					*size = static_cast<int>(n);
				}
				buf_[n] = '\0';
			} else {
				err = errno;
			}
		}
		return err;
	}

	const char* buffer() const { return buf_; }

	static constexpr int BUFFER_SIZE = 64 * 1024;

private:
	int fd_;
	int err_;
	char buf_[BUFFER_SIZE];
};


template <typename String>
int readFile(StringArg filename, int maxSize, String* content, int64_t* fileSize = nullptr, int64_t* modifyTime = nullptr, int64_t* createTime = nullptr)
{
	ReadSmallFile file(filename);
	return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}


//! not thread safe
class AppendToFile : noncopyable
{
public:
	explicit AppendToFile(StringArg filename)
	{

	}

	static constexpr int BUFFER_SIZE = 64 * 1024;


private:
	size_t write(const char* logLine, size_t len) {

	}

	FILE* fp_;
	char buf_[BUFFER_SIZE];
	off_t writtenBytes_;
};

}
}
