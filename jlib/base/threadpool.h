#pragma once

#include "config.h"
#include "noncopyable.h"
#include <mutex>
#include <thread>
#include <functional>
#include <vector>
#include <deque>
#include <string>
#include <exception>
#include <stdio.h>
#include <assert.h>

namespace jlib
{

class ThreadPool : noncopyable
{
public:
	typedef std::function<void()> Task;

	explicit ThreadPool(const std::string& name = "ThreadPool")
		: mutex_()
		, notEmpty_()
		, notFull_()
		, name_(name)
		, maxQueueSize_(0)
		, running_(false)
	{}

	~ThreadPool() {
		if (running_) {
			stop();
		}
	}

	//! must be called before start()
	void setMaxQueueSize(size_t size) { maxQueueSize_ = size; }
	//! must be called before start()
	void setThreadInitCallback(const Task& cb) { threadInitCallback_ = cb; }

	void start(int nThreads) {
		assert(threads_.empty());
		running_ = true;
		for (int i = 0; i < nThreads; i++) {
			threads_.emplace_back(std::thread(std::bind(&ThreadPool::runInThread, this)));
		}
		if (nThreads == 0 && threadInitCallback_) {
			threadInitCallback_();
		}
	}

	void stop() {
		{
			std::lock_guard<std::mutex> lock(mutex_);
			running_ = false;
		}

		notEmpty_.notify_all();

		for (auto& t : threads_) {
			t.join();
		}
	}

	const std::string& name() const { return name_; }

	size_t queueSize() const {
		std::lock_guard<std::mutex> lock(mutex_);
		return taskQueue_.size();
	}

	void run(Task task) {
		if (threads_.empty()) {
			task();
		} else {
			{
				std::unique_lock<std::mutex> lock(mutex_);
				notFull_.wait(lock, [this]() { return !isFull(); });
			}

			std::lock_guard<std::mutex> lock(mutex_);
			assert(!isFull());
			taskQueue_.emplace_back(std::move(task));			
			notEmpty_.notify_one();
		}
	}

protected:
	bool isFull() const {
		return maxQueueSize_ > 0 && taskQueue_.size() >= maxQueueSize_;
	}

	Task take() {
		{
			std::unique_lock<std::mutex> lock(mutex_);
			notEmpty_.wait(lock, [this]() { return !(taskQueue_.empty() && running_); });
		}
		
		std::lock_guard<std::mutex> lock(mutex_);
		Task task;
		if (!taskQueue_.empty()) {
			task = taskQueue_.front();
			taskQueue_.pop_front();
			if (maxQueueSize_ > 0) {
				notFull_.notify_one();
			}
		}
		return task;
	}

	void runInThread() {
		try {
			if (threadInitCallback_) {
				threadInitCallback_();
			}

			while (running_) {
				Task task(take());
				if (task) {
					task();
				}
			}
		} catch (const std::exception & ex) {
			fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
			fprintf(stderr, "reason: %s\n", ex.what());
			abort();
		} catch (...) {
			fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
			throw; // rethrow
		}
	}

private:
	mutable std::mutex mutex_;
	std::condition_variable notEmpty_;
	std::condition_variable notFull_;
	std::string name_;
	Task threadInitCallback_;
	std::vector<std::thread> threads_;
	std::deque<Task> taskQueue_;
	size_t maxQueueSize_;
	bool running_;


};

}
