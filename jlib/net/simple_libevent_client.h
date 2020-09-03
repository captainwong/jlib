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


class simple_libevent_client
{
public:
	typedef void(*OnConnectinoCallback)(bool up, const std::string& msg, void* user_data);

	// return > 0 for ate
	// return 0 for stop
	typedef size_t(*OnMessageCallback)(const char* data, size_t len, void* user_data);

	typedef void(*OnTimerCallback)(void* user_data);


public:
	simple_libevent_client() {}
	virtual ~simple_libevent_client() { stop(); }

	void setUserData(void* d) { userData_ = d; }
	void setOnConnectionCallback(OnConnectinoCallback cb) { onConn_ = cb; }
	void setOnMsgCallback(OnMessageCallback cb) { onMsg_ = cb; }
	// strictTimer 是否严格以超时时间计算回调，否则会计算上次通信时间间隔
	void setOnTimerCallback(OnTimerCallback cb, int seconds, bool strictTimer = false) { 
		onTimer_ = cb; if (seconds > 0) { timeout_ = seconds; } strictTimer_ = strictTimer;
	}
	void setAutoReconnect(bool b) { autoReconnect_ = b; }
	bool start(const std::string& ip, uint16_t port, std::string& msg);
	void stop();
	void send(const char* data, size_t len);
	bool isStarted() const { return started_; }
	bool isConnected() const { return connected_; }

protected:
	bool started_ = false;
	bool connected_ = false;
	bool autoReconnect_ = false;
	void* userData_ = nullptr;
	OnConnectinoCallback onConn_ = nullptr;
	OnMessageCallback onMsg_ = nullptr;
	OnTimerCallback onTimer_ = nullptr;
	int timeout_ = 5;
	bool strictTimer_ = false;
	std::mutex mutex_ = {};

	std::chrono::steady_clock::time_point lastTimeSendData = {};

	struct Impl;
	Impl* impl_ = nullptr;
};


}
}
