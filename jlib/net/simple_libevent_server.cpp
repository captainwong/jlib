#include "simple_libevent_server.h"
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
# ifdef SIMPLELIBEVENTSERVERLIB
#  include "../log2.h"
#  include "simple_libevent_micros.h"
# else
#  include <jlib/log2.h>
#  include <jlib/net/simple_libevent_micros.h>
# endif
#else // JLIB_DISABLE_LOG
# ifdef SIMPLELIBEVENTSERVERLIB
#  include "../log2micros.h"
# else
#  define init_logger(...)
#  define JLOG_DBUG(...)
#  define JLOG_INFO(...)
#  define JLOG_WARN(...)
#  define JLOG_ERRO(...)
#  define JLOG_CRTC(...)
#  define JLOG_ALL(...)

class range_log {
public:
	range_log() {}
	range_log(const char*) {}
};

#  define AUTO_LOG_FUNCTION

#  define dump_hex(...)
#  define dump_asc(...)
#  define JLOG_HEX(...)
#  define JLOG_ASC(...)
# endif
#endif // JLIB_DISABLE_LOG

namespace jlib {
namespace net {

struct BaseClientPrivateData {
	int thread_id = 0;
	void* bev = nullptr;
	void* timer = nullptr;
	std::chrono::steady_clock::time_point lastTimeComm = {};
};


simple_libevent_server::BaseClient::BaseClient(int fd, void* bev)
	: fd(fd)
	, privateData(new BaseClientPrivateData())
{
	((BaseClientPrivateData*)privateData)->bev = bev;
}

simple_libevent_server::BaseClient::~BaseClient()
{
	delete (BaseClientPrivateData*)privateData;
}

simple_libevent_server::BaseClient* simple_libevent_server::BaseClient::createDefaultClient(int fd, void* bev)
{
	BaseClient* client = new BaseClient(fd, bev);
	return client;
}

void simple_libevent_server::BaseClient::send(const void* data, size_t len)
{
	if (!((BaseClientPrivateData*)privateData)->bev) {
		JLOG_CRTC("BaseClient::send bev is nullptr, #{}", fd);
		return;
	}

	auto output = bufferevent_get_output((bufferevent*)((BaseClientPrivateData*)privateData)->bev);
	if (!output) {
		JLOG_INFO("BaseClient::send bev output nullptr, #{}", fd);
		return;
	}

	evbuffer_lock(output);
	evbuffer_add(output, data, len);
	evbuffer_unlock(output);
}

void simple_libevent_server::BaseClient::shutdown(int what)
{
	if (fd != 0) {
		::shutdown(fd, what);
		//fd = 0;
	}
}

void simple_libevent_server::BaseClient::updateLastTimeComm()
{
	((BaseClientPrivateData*)privateData)->lastTimeComm = std::chrono::steady_clock::now();
}

struct simple_libevent_server::PrivateImpl
{
	struct WorkerThreadContext {
		std::string name = {};
		int thread_id = 0;
		event_base* base = nullptr;
		std::thread thread = {};

		static void dummy_timercb_avoid_worker_exit(evutil_socket_t, short, void*)
		{}

		explicit WorkerThreadContext(const std::string& name, int thread_id)
			: name(name)
			, thread_id(thread_id)
		{
			thread = std::thread(&WorkerThreadContext::worker, this);
		}

		void worker() {
			JLOG_INFO("{} WorkerThread #{} started", name.data(), thread_id);
			base = event_base_new();
			timeval tv = { 1, 0 };
			event_add(event_new(base, -1, EV_PERSIST, dummy_timercb_avoid_worker_exit, nullptr), &tv);
			event_base_dispatch(base);
			JLOG_INFO("{} WorkerThread #{} exited", name.data(), thread_id);
		}

		static void readcb(struct bufferevent* bev, void* user_data)
		{
			char buff[4096];
			auto input = bufferevent_get_input(bev);
			simple_libevent_server* server = (simple_libevent_server*)user_data;
			if (/*server->userData_ && */server->onMsg_) {
				int fd = (int)bufferevent_getfd(bev);
				BaseClient* client = nullptr;
				{
					std::lock_guard<std::mutex> lg(server->mutex);
					auto iter = server->clients.find(fd);
					if (iter != server->clients.end()) {
						client = iter->second;
					}
				}
				if (client) {
					while (1) {
						int len = (int)evbuffer_copyout(input, buff, std::min(sizeof(buff), evbuffer_get_length(input)));
						if (len > 0) {
							size_t ate = server->onMsg_(buff, len, client, server->userData_);
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

		static void eventcb(struct bufferevent* bev, short events, void* user_data)
		{
			simple_libevent_server* server = (simple_libevent_server*)user_data;
			//printf("eventcb events=%d %s\n", events, eventToString(events).data());

			std::string msg;
			if (events & (BEV_EVENT_EOF)) {
				msg = ("Connection closed");
			} else if (events & BEV_EVENT_ERROR) {
				msg = ("Got an error on the connection: ");
				msg += strerror(errno);
			}
			int fd = (int)bufferevent_getfd(bev);
			BaseClient* client = nullptr;
			{
				std::lock_guard<std::mutex> lg(server->mutex);
				auto iter = server->clients.find(fd);
				if (iter != server->clients.end()) {
					client = iter->second;
				} else {
					JLOG_CRTC("eventcb cannot find client by fd #{}", (int)fd);
				}
			}
			if (client) {
				if (((BaseClientPrivateData*)client->privateData)->timer) {
					event_del((event*)((BaseClientPrivateData*)client->privateData)->timer);
				}
				if (/*server->userData_ && */server->onConn_) {
					server->onConn_(false, msg, client, server->userData_);
				}
				{
					std::lock_guard<std::mutex> lg(server->mutex);
					server->clients.erase(fd);
					delete client;
				}
			}

			bufferevent_free(bev);
		}
	};
	typedef WorkerThreadContext* WorkerThreadContextPtr;

	PrivateImpl(void* user_data)
		: user_data(user_data)
	{}

	event_base* base = nullptr;
	void* user_data = nullptr;
	std::thread thread = {};
	timeval tv = {};
	WorkerThreadContextPtr* workerThreadContexts = {};
	int curWorkerId = 0;

	static void accpet_error_cb(evconnlistener* listener, void* context)
	{
		auto server = (simple_libevent_server*)context;
		auto base = evconnlistener_get_base(listener);
		int err = EVUTIL_SOCKET_ERROR();
		JLOG_CRTC("{} accpet_error_cb:{}:{}", server->name_, err, evutil_socket_error_to_string(err));
		event_base_loopexit(base, nullptr);
	}

	static void timercb(evutil_socket_t fd, short, void* user_data)
	{
		auto server = (simple_libevent_server*)user_data;
		std::lock_guard<std::mutex> lg(server->mutex);
		auto iter = server->clients.find((int)fd);
		if (iter != server->clients.end()) {
			auto client = iter->second;
			auto now = std::chrono::steady_clock::now();
			auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - ((BaseClientPrivateData*)client->privateData)->lastTimeComm);
			if (diff.count() > server->maxIdleTime_) {
				JLOG_INFO("{} client #{} timeout={}s > {}s, shutting down", server->name_, client->fd, diff.count(), server->maxIdleTime_);
				((BaseClientPrivateData*)client->privateData)->timer = nullptr;
				client->shutdown();
			} else {
				((BaseClientPrivateData*)client->privateData)->timer = event_new(server->impl->base, fd, 0, timercb, server);
				event_add((event*)((BaseClientPrivateData*)client->privateData)->timer, &server->impl->tv);
			}
		} else {
			JLOG_CRTC("{} timercb cannot find client by fd #{}", server->name_, (int)fd);
		}
	}

	static void accept_cb(evconnlistener* listener, evutil_socket_t fd, sockaddr* addr, int socklen, void* user_data)
	{
		char str[INET_ADDRSTRLEN] = { 0 };
		auto sin = (sockaddr_in*)addr;
		inet_ntop(AF_INET, &sin->sin_addr, str, INET_ADDRSTRLEN);

		simple_libevent_server* server = (simple_libevent_server*)user_data;
		auto ctx = server->impl->workerThreadContexts[server->impl->curWorkerId];

		auto bev = bufferevent_socket_new(ctx->base, fd, BEV_OPT_CLOSE_ON_FREE);
		if (!bev) {
			JLOG_CRTC("{} Error constructing bufferevent!", server->name_);
			exit(-1);
		}

		assert(server->newClient_);
		auto client = server->newClient_((int)fd, bev);
		((BaseClientPrivateData*)client->privateData)->thread_id = server->impl->curWorkerId;
		client->ip = str;
		client->port = sin->sin_port;
		client->updateLastTimeComm();
		((BaseClientPrivateData*)client->privateData)->timer = event_new(ctx->base, fd, 0, timercb, /*new TimerContext({ server, client })*/ server);
		event_add((event*)((BaseClientPrivateData*)client->privateData)->timer, &server->impl->tv);

		{
			std::lock_guard<std::mutex> lg(server->mutex);
			server->clients[(int)fd] = client;
		}

		bufferevent_setcb(bev, WorkerThreadContext::readcb, nullptr, WorkerThreadContext::eventcb, server);
		bufferevent_enable(bev, EV_WRITE | EV_READ);

		if (/*server->userData_ && */server->onConn_) {
			server->onConn_(true, "", client, server->userData_);
		}

		server->impl->curWorkerId = (server->impl->curWorkerId + 1) % server->threadNum_;
	}

};

simple_libevent_server::simple_libevent_server()
{
	AUTO_LOG_FUNCTION;
	SIMPLE_LIBEVENT_ONE_TIME_INITTER;
}

simple_libevent_server::~simple_libevent_server()
{
	AUTO_LOG_FUNCTION;
	stop();
}

bool simple_libevent_server::start(uint16_t port, std::string& msg)
{
	AUTO_LOG_FUNCTION;
	do {
		stop();

		std::lock_guard<std::mutex> lg(mutex);

		impl = new PrivateImpl(this);
		impl->base = event_base_new();
		if (!impl->base) {
			msg = name_ + " init libevent failed";
			JLOG_CRTC(msg);
			break;
		}

		sockaddr_in sin = { 0 };
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = htonl(INADDR_ANY);
		sin.sin_port = htons(port);

		auto listener = evconnlistener_new_bind(impl->base,
												PrivateImpl::accept_cb,
												this,
												LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
												-1, // backlog, -1 for default
												(const sockaddr*)(&sin),
												sizeof(sin));
		if (!listener) {
			msg = name_ + " create listener failed";
			JLOG_CRTC(msg);
			break;
		}
		evconnlistener_set_error_cb(listener, PrivateImpl::accpet_error_cb);

		// init common timeout 
		impl->tv.tv_sec = maxIdleTime_;
		impl->tv.tv_usec = 0;
		const struct timeval* tv_out = event_base_init_common_timeout(impl->base, &impl->tv);
		memcpy(&impl->tv, tv_out, sizeof(struct timeval));		

		impl->workerThreadContexts = new PrivateImpl::WorkerThreadContextPtr[threadNum_];
		for (int i = 0; i < threadNum_; i++) {
			impl->workerThreadContexts[i] = (new PrivateImpl::WorkerThreadContext(name_, i));
		}

		// fix 
		// wait till all worker thread's base is created
		bool all_created = false;
		while (!all_created) {
			all_created = true;
			for (int i = 0; i < threadNum_; i++) {
				if (!impl->workerThreadContexts[i]->base) {
					all_created = false;
					break;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		impl->thread = std::thread([this]() {
			JLOG_INFO("{} listen thread started", name_.data());
			event_base_dispatch(this->impl->base);
			JLOG_INFO("{} listen thread exited", name_.data());
		});

		started_ = true;
		return true;
	} while (0);

	stop();
	return false;
}

void simple_libevent_server::stop()
{
	AUTO_LOG_FUNCTION;
	std::lock_guard<std::mutex> lg(mutex);
	if (!impl) { return; }

	timeval tv{ 0, 1000 };

	if (impl->base) {
		event_base_loopexit(impl->base, &tv);
	}

	if (impl->thread.joinable()) {
		impl->thread.join();
	}

	if (impl->base) {
		event_base_free(impl->base);
		impl->base = nullptr;
	}

	if (impl->workerThreadContexts) {
		for (int i = 0; i < threadNum_; i++) {
			event_base_loopexit(impl->workerThreadContexts[i]->base, &tv);
		}

		for (int i = 0; i < threadNum_; i++) {
			impl->workerThreadContexts[i]->thread.join();
			event_base_free(impl->workerThreadContexts[i]->base);
			delete impl->workerThreadContexts[i];
		}

		delete impl->workerThreadContexts;
	}

	delete impl;
	impl = nullptr;

	for (auto client : clients) {
		delete client.second;
	}
	clients.clear();

	started_ = false;
}

}
}
