#include "simple_libevent_clients.h"
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/thread.h>
#include <thread>
#include <mutex>
#include <algorithm>
#include <signal.h>
#include <inttypes.h>

#if defined(DISABLE_JLIB_LOG2) && !defined(JLIB_DISABLE_LOG)
#define JLIB_DISABLE_LOG
#endif

#ifndef JLIB_DISABLE_LOG
# ifdef SIMPLELIBEVENTCLIENTSLIB
#  include "../log2.h"
#  include "simple_libevent_micros.h"
# else
#  include <jlib/log2.h>
#  include <jlib/net/simple_libevent_micros.h>
# endif
#else // JLIB_DISABLE_LOG
#define init_logger(...)
#define JLOG_DBUG(...)
#define JLOG_INFO(...)
#define JLOG_WARN(...)
#define JLOG_ERRO(...)
#define JLOG_CRTC(...)
#define JLOG_ALL(...)

class range_log {
public:
	range_log() {}
	range_log(const char*) {}
};

#define AUTO_LOG_FUNCTION

#define dump_hex(...)
#define dump_asc(...)
#define JLOG_HEX(...)
#define JLOG_ASC(...)
#endif // JLIB_DISABLE_LOG


namespace jlib {
namespace net {

struct simple_libevent_clients::BaseClient::PrivateData {
	int thread_id = 0;
	int client_id = 0;
	int fd = 0;
	bufferevent* bev = nullptr;
	event* timer = nullptr;
	int timeout = 5;
	OnTimerCallback on_timer = nullptr;
	void* user_data = nullptr;
	int lifetime = -1;
	event* lifetimer = nullptr;
	std::string server_ip{};
	uint16_t server_port = 0;
	bool auto_reconnect = false;
	std::chrono::steady_clock::time_point lastTimeComm = {};
};


simple_libevent_clients::BaseClient::BaseClient()
	: privateData(new PrivateData())
{
}

simple_libevent_clients::BaseClient::~BaseClient()
{
	delete privateData;
}

simple_libevent_clients::BaseClient* simple_libevent_clients::BaseClient::createDefaultClient()
{
	return new BaseClient();
}

int simple_libevent_clients::BaseClient::thread_id() const
{
	return privateData->thread_id;
}

int simple_libevent_clients::BaseClient::client_id() const
{
	return privateData->client_id;
}

int simple_libevent_clients::BaseClient::fd() const
{
	return privateData->fd;
}

bool simple_libevent_clients::BaseClient::auto_reconnect() const
{
	return privateData->auto_reconnect;
}

std::string simple_libevent_clients::BaseClient::server_ip() const
{
	return privateData->server_ip;
}

int16_t simple_libevent_clients::BaseClient::server_port() const
{
	return privateData->server_port;
}

int simple_libevent_clients::BaseClient::lifetime() const
{
	return privateData->lifetime;
}

void simple_libevent_clients::BaseClient::send(const void* data, size_t len)
{
	if (!privateData->bev) {
		JLOG_CRTC("BaseClient::send bev is nullptr, #{}", fd());
		return;
	}

	auto output = bufferevent_get_output(privateData->bev);
	if (!output) {
		JLOG_INFO("BaseClient::send bev output nullptr, #{}", fd());
		return;
	}

	evbuffer_lock(output);
	evbuffer_add(output, data, len);
	evbuffer_unlock(output);
}

void simple_libevent_clients::BaseClient::shutdown(int what)
{
	if (fd() != 0) {
		::shutdown(fd(), what);
		//fd = 0;
	}
}

void simple_libevent_clients::BaseClient::updateLastTimeComm()
{
	privateData->lastTimeComm = std::chrono::steady_clock::now();
}

void simple_libevent_clients::BaseClient::set_auto_reconnect(bool b)
{
	privateData->auto_reconnect = b;
}

struct simple_libevent_clients::PrivateImpl
{
	struct WorkerThreadContext {
		simple_libevent_clients* ctx = nullptr;
		int thread_id = 0;
		std::string name{};
		event_base* base = nullptr;
		std::mutex mutex{};
		// fd => client*
		std::unordered_map<int, simple_libevent_clients::BaseClient*> clients{};
		int clients_to_connect = 0;
		int client_id_to_connect = 0;

		static void dummy_timercb_avoid_worker_exit(evutil_socket_t, short, void*)
		{}

		explicit WorkerThreadContext(simple_libevent_clients* ctx, int thread_id, const std::string& name = {})
			: ctx(ctx)
			, thread_id(thread_id)
			, name(name)
		{
			base = event_base_new();
		}

		void worker() {
			JLOG_INFO("{} WorkerThread #{} started", name, thread_id);
			timeval tv = { 1, 0 };
			event_add(event_new(base, -1, EV_PERSIST, dummy_timercb_avoid_worker_exit, nullptr), &tv);
			event_base_dispatch(base);
			JLOG_INFO("{} WorkerThread #{} exited", name.data(), thread_id);
		}

		bool connect(const std::string& ip, uint16_t port, std::string& msg) {
			std::lock_guard<std::mutex> lg(mutex);
			auto bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
			if (!bev) {
				msg = ("allocate bufferevent failed");
				return false;
			}
			auto client = ctx->newClient_();
			client->privateData->bev = bev;
			client->privateData->thread_id = thread_id;
			client->privateData->client_id = client_id_to_connect++;

			bufferevent_setcb(bev, readcb, writecb, eventcb, this);
			bufferevent_enable(bev, EV_READ | EV_WRITE);

			sockaddr_in addr = { 0 };
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = inet_addr(ip.data());
			addr.sin_port = htons(port);
			if (bufferevent_socket_connect(bev, (const sockaddr*)(&addr), sizeof(addr)) < 0) {
				client->privateData->fd = (int)bufferevent_getfd(bev);
				int err = evutil_socket_geterror(client->privateData->fd);
				msg = "error starting connection: " + std::to_string(err) + evutil_socket_error_to_string(err);
				delete client;
				return false;
			}
			client->privateData->fd = (int)bufferevent_getfd(bev);
			clients[client->privateData->fd] = client;
			return true;
		}

		static void readcb(struct bufferevent* bev, void* user_data)
		{
			char buff[4096];
			auto input = bufferevent_get_input(bev);
			WorkerThreadContext* context = (WorkerThreadContext*)user_data;
			if (context->ctx->onMsg_) {
				int fd = (int)bufferevent_getfd(bev);
				simple_libevent_clients::BaseClient* client = nullptr;
				{
					std::lock_guard<std::mutex> lg(context->mutex);
					auto iter = context->clients.find(fd);
					if (iter != context->clients.end()) {
						client = iter->second;
					}
				}
				if (client) {
					while (1) {
						int len = (int)evbuffer_copyout(input, buff, std::min(sizeof(buff), evbuffer_get_length(input)));
						if (len > 0) {
							size_t ate = context->ctx->onMsg_(buff, len, client, context->ctx->userData_);
							if (ate > 0) {
								evbuffer_drain(input, ate);
								continue;
							}
						}
						break;
					}
				} else {
					bufferevent_free(bev);
				}
			} else {
				evbuffer_drain(input, evbuffer_get_length(input));
			}
		}

		static void writecb(struct bufferevent* bev, void* user_data)
		{
			WorkerThreadContext* context = (WorkerThreadContext*)user_data;
			simple_libevent_clients::BaseClient* client = nullptr;
			{
				std::lock_guard<std::mutex> lg(context->mutex);
				int fd = (int)bufferevent_getfd(bev);
				auto iter = context->clients.find(fd);
				if (iter != context->clients.end()) {
					client = iter->second;
				}
			}
			if (client && context->ctx->onWrite_) {
				context->ctx->onWrite_(client, context->ctx->userData_);
			}
		}

		static void eventcb(struct bufferevent* bev, short events, void* user_data)
		{
			WorkerThreadContext* context = (WorkerThreadContext*)user_data;
			JLOG_DBUG("eventcb events={} {}", events, eventToString(events));

			bool up = false;
			std::string msg;
			int fd = (int)bufferevent_getfd(bev);

			if (events & BEV_EVENT_CONNECTED) {
				up = true;
			} else if (events & (BEV_EVENT_EOF)) {
				msg = ("Connection closed");
			} else if (events & BEV_EVENT_ERROR) {
				msg = ("Got an error on the connection: ");
				msg += strerror(errno);
			}
			
			BaseClient* client = nullptr;
			{
				std::lock_guard<std::mutex> lg(context->mutex);
				auto iter = context->clients.find(fd);
				if (iter != context->clients.end()) {
					client = iter->second;
				} else {
					JLOG_CRTC("eventcb cannot find client by fd #{}", (int)fd);
				}
			}

			if (client) {				
				if (context->ctx->onConn_) {
					context->ctx->onConn_(up, msg, client, context->ctx->userData_);
				}
				if (!up) {
					if (client->privateData->timer) {
						event_del(client->privateData->timer);
						client->privateData->timer = nullptr;
					}
					std::lock_guard<std::mutex> lg(context->mutex);
					context->clients.erase(fd);
					delete client;
				}
			}

			if (!up) {
				bufferevent_free(bev);
			}
		}
	};
	typedef WorkerThreadContext* WorkerThreadContextPtr;
	std::vector<WorkerThreadContextPtr> contexts{};
	std::vector<std::thread> threads{};
	simple_libevent_clients* ctx = nullptr;

	PrivateImpl(simple_libevent_clients* ctx, int threadNum, const std::string& name = {})
		: ctx(ctx)
	{
		for (int i = 0; i < threadNum; i++) {
			auto context = new WorkerThreadContext(ctx, i, name);
			contexts.push_back(context);
			threads.emplace_back(std::thread(&WorkerThreadContext::worker, context));
		}
	}

	~PrivateImpl() {
		for (auto context : contexts) {
			timeval tv{ 0, 1000 };
			event_base_loopexit(context->base, &tv);			
		}
		for (auto& t : threads) {
			t.join();
		}
		threads.clear();
		for (auto context : contexts) {
			event_base_free(context->base);
			context->clients.clear();
			delete context;
		}
		contexts.clear();
	}

	static void timercb(evutil_socket_t fd, short, void* user_data)
	{
		auto client = (BaseClient*)user_data;
		if (client->privateData->on_timer) {
			client->privateData->on_timer(client, client->privateData->user_data);
		}
		client->privateData->timer = event_new(bufferevent_get_base(client->privateData->bev), client->fd(), 0, timercb, client);
		struct timeval tv = { client->privateData->timeout, 0 };
		event_add(client->privateData->timer, &tv);
	}

	static void lifetimer_cb(evutil_socket_t fd, short, void* user_data)
	{
		auto client = (BaseClient*)user_data;
		client->shutdown();
	}

};

simple_libevent_clients::simple_libevent_clients(OnConnectinoCallback onConn, OnMessageCallback onMsg, OnWriteCompleteCallback onWrite,
												 NewClientCallback newClient,  int threads, void* user_data, const std::string& name)
	: onConn_(onConn)
	, onMsg_(onMsg)
	, onWrite_(onWrite)
	, newClient_(newClient)
	, threadNum_(threads >= 1 ? threads : 1)
	, userData_(user_data)
	, name_(name)

{
	SIMPLE_LIBEVENT_ONE_TIME_INITTER;
}

simple_libevent_clients::~simple_libevent_clients()
{
	exit();
}

bool simple_libevent_clients::connect(const std::string& ip, uint16_t port, std::string& msg)
{
	std::lock_guard<std::mutex> lg(mutex_);
	if (!impl) {
		impl = new PrivateImpl(this, threadNum_, name_);
	}
	curThreadId_ = ++curThreadId_ % threadNum_;
	return impl->contexts[curThreadId_]->connect(ip, port, msg);
}

void simple_libevent_clients::exit()
{
	std::lock_guard<std::mutex> lg(mutex_);
	if (!impl) { return; }

	delete impl;
	impl = nullptr;
}


void simple_libevent_clients::BaseClient::set_timer(OnTimerCallback cb, void* user_data, int seconds)
{
	if (privateData->timer) {
		event_del(privateData->timer);
	}
	privateData->on_timer = cb;
	privateData->user_data = user_data;
	privateData->timeout = seconds;
	auto base = bufferevent_get_base(privateData->bev);
	privateData->timer = event_new(base, fd(), 0, PrivateImpl::timercb, this);
	if (!privateData->timer) {
		JLOG_CRTC("create timer failed");
		return;
	}
	struct timeval tv = { seconds, 0 };
	event_add(privateData->timer, &tv);
}

void simple_libevent_clients::BaseClient::set_lifetime(int seconds)
{
	privateData->lifetime = seconds;
	if (privateData->lifetimer) {
		event_del(privateData->lifetimer);
	}
	auto base = bufferevent_get_base(privateData->bev);
	privateData->lifetimer = event_new(base, fd(), 0, PrivateImpl::lifetimer_cb, this);
	if (!privateData->lifetimer) {
		JLOG_CRTC("create lifetimer failed");
		return;
	}
	struct timeval tv = { seconds, 0 };
	event_add(privateData->lifetimer, &tv);
}

}
}
