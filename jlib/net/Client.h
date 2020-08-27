#pragma once

# ifndef  _CRT_SECURE_NO_WARNINGS
#  define  _CRT_SECURE_NO_WARNINGS
# endif
# ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#  define _WINSOCK_DEPRECATED_NO_WARNINGS
# endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <string>
#include <stdint.h>
#include <mutex>
#include <vector>
#include <chrono>

namespace jlib {
namespace net {


typedef void(*OnConnectinoCallback)(bool up, const std::string& msg, void* user_data);

// return > 0 for ate
// return 0 for stop
typedef size_t(*OnMessageCallback)(const char* data, size_t len, void* user_data);

typedef void(*OnTimerCallback)(void* user_data);

class Client
{
public:
	Client() {}
	virtual ~Client() { stop(); }

	void setUserData(void* d) { userData_ = d; }
	void setOnConnectionCallback(OnConnectinoCallback cb) { onConn_ = cb; }
	void setOnMsgCallback(OnMessageCallback cb) { onMsg_ = cb; }
	void setOnTimerCallback(OnTimerCallback cb, int seconds) { onTimer_ = cb; if (seconds > 0) { timeout_ = seconds; } }
	void setAutoReconnect(bool b) { autoReconnect_ = b; }
	bool start(const std::string& ip, uint16_t port, std::string& msg);
	void stop();
	void send(const char* data, size_t len);
	bool isStarted() const { return started_; }

protected:
	bool started_ = false;
	bool autoReconnect_ = false;
	void* userData_ = nullptr;
	OnConnectinoCallback onConn_ = nullptr;
	OnMessageCallback onMsg_ = nullptr;
	OnTimerCallback onTimer_ = nullptr;
	int timeout_ = 5;
	std::mutex mutex_ = {};

	std::chrono::steady_clock::time_point lastTimeSendData = {};

	struct Impl;
	Impl* impl_ = nullptr;
};


}
}
