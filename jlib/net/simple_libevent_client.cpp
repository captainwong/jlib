#include "simple_libevent_client.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include <thread>
#include <mutex>

#if defined(DISABLE_JLIB_LOG2) && !defined(JLIB_DISABLE_LOG)
#define JLIB_DISABLE_LOG
#endif

#ifndef JLIB_DISABLE_LOG
# ifdef SIMPLELIBEVENTCLIENTLIB
#  include "../log2.h"
#  include "simple_libevent_micros.h"
# else
#  include <jlib/log2.h>
#  include <jlib/net/simple_libevent_micros.h>
# endif
#else // JLIB_DISABLE_LOG
# ifdef SIMPLELIBEVENTCLIENTLIB
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



struct simple_libevent_client::Impl
{
	Impl(void* user_data)
		: user_data(user_data)
	{
		SIMPLE_LIBEVENT_ONE_TIME_INITTER;
	}

	event_base* base = nullptr;
	bufferevent* bev = nullptr;
	event* timer = nullptr;
	void* user_data = nullptr;
	std::string ip = {};
	uint16_t port = 0;
	std::thread thread = {};

	static void writecb(struct bufferevent*, void* user_data)
	{
		simple_libevent_client* client = (simple_libevent_client*)user_data;
		client->lastTimeSendData = std::chrono::steady_clock::now();
	}

	static void readcb(struct bufferevent* bev, void* user_data)
	{
		char buff[4096];
		auto input = bufferevent_get_input(bev);
		JLOG_DBUG("readcb, readable len {}", evbuffer_get_length(input));
		simple_libevent_client* client = (simple_libevent_client*)user_data;
		if (client->userData_ && client->onMsg_) {
			while (1) {
				int len = evbuffer_copyout(input, buff, std::min(sizeof(buff), evbuffer_get_length(input)));
				if (len > 0) {
					size_t ate = client->onMsg_(buff, len, client->userData_);
					if (ate > 0) {
						evbuffer_drain(input, ate);
						continue;
					}
				}
				break;
			}
		} else {
			evbuffer_drain(input, evbuffer_get_length(input));
		}
	}

	static void eventcb(struct bufferevent* bev, short events, void* user_data)
	{
		simple_libevent_client* client = (simple_libevent_client*)user_data;
		JLOG_DBUG("eventcb events={}", eventToString(events));

		std::string msg;
		if (events & BEV_EVENT_CONNECTED) {
			client->connected_ = true;
			if (client->userData_ && client->onConn_) {
				client->onConn_(true, "connected", client->userData_);
			}

			client->lastTimeSendData = std::chrono::steady_clock::now();

			if (client->strictTimer_) {
				struct timeval tv = { client->timeout_, 0 };
				event_add(event_new(client->impl_->base, -1, 0, Impl::timercb, client), &tv);
			} else {
				struct timeval tv = { 1, 0 };
				event_add(event_new(client->impl_->base, -1, 0, Impl::timercb, client), &tv);
			}
			return;
		} else if (events & (BEV_EVENT_EOF)) {
			msg = ("Connection closed");
		} else if (events & BEV_EVENT_ERROR) {
			msg = ("Connection closed, got an error on the connection: ");
			msg += strerror(errno);
		}

		if (client->impl_->timer) {
			event_del(client->impl_->timer);
			client->impl_->timer = nullptr;
		}

		client->impl_->bev = nullptr;

		client->connected_ = false;
		if (client->userData_ && client->onConn_) {
			client->onConn_(false, msg, client->userData_);
		}
		bufferevent_free(bev);

		if (client->autoReconnect_) {
			struct timeval tv = { 3, 0 };
			event_add(event_new(client->impl_->base, -1, 0, Impl::reconn_timercb, client), &tv);
		}
	}

	static void timercb(evutil_socket_t, short, void* user_data)
	{
		simple_libevent_client* client = (simple_libevent_client*)user_data;
		struct timeval tv = { 1, 0 };
		if (client->strictTimer_) {
			if (client->userData_ && client->onTimer_) {
				client->onTimer_(client->userData_);
			}
			tv.tv_sec = client->timeout_;			
		} else {
			auto now = std::chrono::steady_clock::now();
			auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - client->lastTimeSendData);
			if (diff.count() >= client->timeout_) {
				if (client->userData_ && client->onTimer_) {
					client->onTimer_(client->userData_);
				}
			}
		}		

		if (client->connected_) {
			client->impl_->timer = event_new(client->impl_->base, -1, 0, Impl::timercb, client);
			event_add(client->impl_->timer, &tv);
		}
	}

	static void reconn_timercb(evutil_socket_t, short, void* user_data)
	{
		AUTO_LOG_FUNCTION;
		simple_libevent_client* client = (simple_libevent_client*)user_data;

		do {
			std::string msg = "Reconnecting " + client->impl_->ip + ":" + std::to_string(client->impl_->port);
			/*if (client->userData_ && client->onConn_) {
				client->onConn_(false, msg, client->userData_);
			}*/

			sockaddr_in sin = { 0 };
			sin.sin_family = AF_INET;
			sin.sin_addr.s_addr = inet_addr(client->impl_->ip.data());
			sin.sin_port = htons(client->impl_->port);

			client->impl_->bev = bufferevent_socket_new(client->impl_->base, -1, BEV_OPT_CLOSE_ON_FREE);
			if (!client->impl_->bev) {
				msg = ("Allocate bufferevent failed");
				if (client->userData_ && client->onConn_) {
					client->onConn_(false, msg, client->userData_);
				}
				break;
			}
			bufferevent_setcb(client->impl_->bev, Impl::readcb, Impl::writecb, Impl::eventcb, client);
			bufferevent_enable(client->impl_->bev, EV_READ | EV_WRITE);

			if (bufferevent_socket_connect(client->impl_->bev, (sockaddr*)(&sin), sizeof(sin)) < 0) {
				msg = ("Error starting connection:");
				msg += strerror(errno);;
				if (client->userData_ && client->onConn_) {
					client->onConn_(false, msg, client->userData_);
				}

				return;
			}
		} while (0);
	}
};

bool simple_libevent_client::start(const std::string& ip, uint16_t port, std::string& msg, bool start_in_thread)
{
	AUTO_LOG_FUNCTION;
	do {
		stop();

		//std::lock_guard<std::mutex> lg(mutex_);
		mutex_.lock();

		impl_ = new Impl(this);
		impl_->ip = ip;
		impl_->port = port;

		impl_->base = event_base_new();
		if (!impl_->base) {
			msg = "init libevent failed";
			mutex_.unlock();
			break;
		}

		sockaddr_in sin = { 0 };
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = inet_addr(ip.data());
		sin.sin_port = htons(port);

		impl_->bev = bufferevent_socket_new(impl_->base, -1, BEV_OPT_CLOSE_ON_FREE);
		if (!impl_->bev) {
			msg = ("allocate bufferevent failed");
			mutex_.unlock();
			break;
		}
		bufferevent_setcb(impl_->bev, Impl::readcb, Impl::writecb, Impl::eventcb, this);
		bufferevent_enable(impl_->bev, EV_READ | EV_WRITE);

		if (bufferevent_socket_connect(impl_->bev, (sockaddr*)(&sin), sizeof(sin)) < 0) {
			msg = ("error starting connection");
			mutex_.unlock();
			break;
		}

		lastTimeSendData = std::chrono::steady_clock::now();

		if (start_in_thread) {
			impl_->thread = std::thread([this]() {
				JLOG_WARN("libevent thread started");
				event_base_dispatch(this->impl_->base);
				JLOG_WARN("libevent thread exited");
			});
			started_ = true;
			mutex_.unlock();
		} else {
			started_ = true;
			mutex_.unlock();
			event_base_dispatch(this->impl_->base);
		}

		return true;
	} while (0);

	stop();
	return false;
}

void simple_libevent_client::stop()
{
	AUTO_LOG_FUNCTION;
	std::lock_guard<std::mutex> lg(mutex_);
	if (!impl_) { return; }

	auto old = autoReconnect_;
	autoReconnect_ = false;

	if (impl_->timer) {
		event_del(impl_->timer);
		impl_->timer = nullptr;
	}

	if (impl_->bev) {
		shutdown(bufferevent_getfd(impl_->bev), 1);
		//impl_->bev = nullptr;
	}

	if (impl_->base) {
		timeval tv = { 0, 1000 };
		event_base_loopexit(impl_->base, &tv);
	}

	if (impl_->thread.joinable()) {
		impl_->thread.join();
	}

	if (impl_->bev) {
		impl_->bev = nullptr;
	}
	if (impl_->base) {
		event_base_free(impl_->base);
		impl_->base = nullptr;
	}

	delete impl_;
	impl_ = nullptr;

	started_ = false;
	autoReconnect_ = old;
}

void simple_libevent_client::send(const char* data, size_t len)
{
	std::lock_guard<std::mutex> lg(mutex_);
	if (!impl_ || !impl_->base || !impl_->bev) { return; }
	auto output = bufferevent_get_output(impl_->bev);
	evbuffer_lock(output);
	evbuffer_add(output, data, len);
	evbuffer_unlock(output);
}

}
}
