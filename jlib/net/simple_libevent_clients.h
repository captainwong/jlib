#pragma once

#ifndef _WIN32
#  include <unistd.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <sys/socket.h>
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
#  include <WinSock2.h>
#  pragma comment(lib, "ws2_32.lib")
#endif

#include <stdint.h>
#include <string>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <assert.h>

namespace jlib {
namespace net {

class simple_libevent_clients
{
public:

	struct BaseClient;

	typedef void(*OnTimerCallback)(BaseClient* client, void* user_data);

	typedef BaseClient* (*NewClientCallback)();

	typedef void(*OnConnectinoCallback)(bool up, const std::string& msg, BaseClient* client, void* user_data);

	// return > 0 for ate
	// return 0 for stop
	typedef size_t(*OnMessageCallback)(const char* data, size_t len, BaseClient* client, void* user_data);

	typedef void(*OnWriteCompleteCallback)(BaseClient* client, void* user_data);


	struct BaseClient {
		explicit BaseClient();
		virtual ~BaseClient();

		static BaseClient* createDefaultClient();

		int thread_id() const;
		int client_id() const;
		int fd() const;
		bool auto_reconnect() const;
		std::string server_ip() const;
		int16_t server_port() const;
		int lifetime() const;

		void send(const void* data, size_t len);
		void shutdown(int what = 1);
		void updateLastTimeComm();
		void set_auto_reconnect(bool b);
		void set_timer(OnTimerCallback cb, void* user_data, int seconds);
		// set to -1 for live until peer disconnected
		void set_lifetime(int seconds);

		struct PrivateData;
		PrivateData* privateData = nullptr;
	};

public:
	explicit simple_libevent_clients(OnConnectinoCallback onConn, OnMessageCallback onMsg, OnWriteCompleteCallback onWrite,
									 NewClientCallback newClient, int threads, void* user_data, const std::string& name = {});
	virtual ~simple_libevent_clients();

	void setUserData(void* user_data) { userData_ = user_data; }

	bool connect(const std::string& ip, uint16_t port, std::string& msg);
	void exit();


	

protected:
	struct PrivateImpl;
	PrivateImpl* impl = nullptr;
	std::string name_{};
	void* userData_ = nullptr;
	OnConnectinoCallback onConn_ = nullptr;
	OnMessageCallback onMsg_ = nullptr;
	OnWriteCompleteCallback onWrite_ = nullptr;
	NewClientCallback newClient_ = BaseClient::createDefaultClient;

	//! 工作线程数量
	int threadNum_ = 1;
	int curThreadId_ = -1;
	std::mutex mutex_{};
};

}
}
