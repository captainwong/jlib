#pragma once

#ifndef _WIN32
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#else
#  ifndef  _CRT_SECURE_NO_WARNINGS
#    define  _CRT_SECURE_NO_WARNINGS
#  endif
#  ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#    define _WINSOCK_DEPRECATED_NO_WARNINGS
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <stdint.h>
#include <string>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <assert.h>

namespace jlib {
namespace net {

class simple_libevent_server
{
public:
	struct BaseClient {
		explicit BaseClient(int fd, void* bev);
		virtual ~BaseClient();

		static BaseClient* createDefaultClient(int fd, void* bev);

		void send(const void* data, size_t len);
		// 0: recv, 1: send, 2: both
		void shutdown(int what = 0);
		void updateLastTimeComm();

		int fd = 0;
		std::string ip = {};
		uint16_t port = 0;
		void* privateData = nullptr;
	};

	typedef BaseClient* (*NewClientCallback)(int fd, void* bev);

	typedef void(*OnConnectinoCallback)(bool up, const std::string& msg, BaseClient* client, void* user_data);

	// return > 0 for ate
	// return 0 for stop
	typedef size_t(*OnMessageCallback)(const char* data, size_t len, BaseClient* client, void* user_data);


public:
	explicit simple_libevent_server();
	virtual ~simple_libevent_server();


	// these functions wont take effect after start() is called
	void setName(const std::string& name) { name_ = name; }
	void setNewClientCallback(NewClientCallback cb) { assert(cb); newClient_ = cb ? cb : BaseClient::createDefaultClient; }
	void setUserData(void* d) { userData_ = d; }
	void setOnConnectionCallback(OnConnectinoCallback cb) { onConn_ = cb; }
	void setOnMsgCallback(OnMessageCallback cb) { onMsg_ = cb; }
	void setClientMaxIdleTime(int sec) { maxIdleTime_ = sec; }
	void setThreadNum(int threads) { assert(threads >= 1); if (threads >= 1) { threadNum_ = threads; } }

	// call above functions before start()
	bool start(uint16_t port, std::string& msg);
	void stop();
	bool isStarted() const { return started_; }

protected:
	struct PrivateImpl;
	PrivateImpl* impl = nullptr;

	std::string name_ = {};
	bool started_ = false;
	void* userData_ = nullptr;
	OnConnectinoCallback onConn_ = nullptr;
	OnMessageCallback onMsg_ = nullptr;
	NewClientCallback newClient_ = BaseClient::createDefaultClient;

	//! 客户端最长无数据时间
	int maxIdleTime_ = 5;

	//! 工作线程数量
	int threadNum_ = 1;

	std::mutex mutex = {};
	std::unordered_map<int, BaseClient*> clients = {};
};

}
}
