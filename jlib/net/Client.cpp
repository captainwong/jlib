#include "Client.h"


#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

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
#include "../log2.h"

#ifndef JLIB_LOG2_ENABLED
#pragma error "jlib::log2 not found!"
#endif

namespace jlib {
namespace net {


struct OneTimeIniter {
	OneTimeIniter() {
		if (0 != evthread_use_windows_threads()) {
			JLOG_CRTC("failed to init libevent with thread by calling evthread_use_windows_threads");
		}
	}
};

struct Client::Impl
{
	Impl(void* user_data)
		: user_data(user_data)
	{
		static OneTimeIniter init;
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
		Client* client = (Client*)user_data;
		client->lastTimeSendData = std::chrono::steady_clock::now();
	}

	static void readcb(struct bufferevent* bev, void* user_data)
	{
		char buff[4096];
		auto input = bufferevent_get_input(bev);
		JLOG_INFO("readcb, readable len {}", evbuffer_get_length(input));
		Client* client = (Client*)user_data;
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

	static std::string eventToString(short evs) {
		std::string s;
#define check_event_append_to_s(e) if(evs & e){s += #e " ";}
		check_event_append_to_s(BEV_EVENT_READING);
		check_event_append_to_s(BEV_EVENT_WRITING);
		check_event_append_to_s(BEV_EVENT_EOF);
		check_event_append_to_s(BEV_EVENT_ERROR);
		check_event_append_to_s(BEV_EVENT_TIMEOUT);
		check_event_append_to_s(BEV_EVENT_CONNECTED);
#undef check_event_append_to_s
		return s;
	}

	static void eventcb(struct bufferevent* bev, short events, void* user_data)
	{
		Client* client = (Client*)user_data;
		JLOG_INFO("eventcb events={}", eventToString(events));

		std::string msg;
		if (events & BEV_EVENT_CONNECTED) {
			if (client->userData_ && client->onConn_) {
				client->onConn_(true, "connected", client->userData_);
			}

			client->lastTimeSendData = std::chrono::steady_clock::now();

			struct timeval tv = { 1, 0 };
			event_add(event_new(client->impl_->base, bufferevent_getfd(bev), 0, Impl::timercb, client), &tv);

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
		if (client->userData_ && client->onConn_) {
			client->onConn_(false, msg, client->userData_);
		}
		bufferevent_free(bev);

		if (client->autoReconnect_) {
			struct timeval tv = { 3, 0 };
			event_add(event_new(client->impl_->base, -1, 0, Impl::reconn_timercb, client), &tv);
		}
	}

	static void timercb(evutil_socket_t fd, short, void* user_data)
	{
		Client* client = (Client*)user_data;
		auto now = std::chrono::steady_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - client->lastTimeSendData);
		if (diff.count() >= client->timeout_) {
			if (client->userData_ && client->onTimer_) {
				client->onTimer_(client->userData_);
			}
		}
		struct timeval tv = { 1, 0 };
		client->impl_->timer = event_new(client->impl_->base, fd, 0, Impl::timercb, client);
		event_add(client->impl_->timer, &tv);
	}

	static void reconn_timercb(evutil_socket_t, short, void* user_data)
	{
		Client* client = (Client*)user_data;

		do {
			std::string msg = "Reconnecting " + client->impl_->ip + ":" + std::to_string(client->impl_->port);
			if (client->userData_ && client->onConn_) {
				client->onConn_(false, msg, client->userData_);
			}

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

bool Client::start(const std::string& ip, uint16_t port, std::string& msg)
{
	do {
		stop();

		std::lock_guard<std::mutex> lg(mutex_);

		impl_ = new Impl(this);
		impl_->ip = ip;
		impl_->port = port;

		impl_->base = event_base_new();
		if (!impl_->base) {
			msg = "init libevent failed";
			break;
		}

		sockaddr_in sin = { 0 };
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = inet_addr(ip.data());
		sin.sin_port = htons(port);

		impl_->bev = bufferevent_socket_new(impl_->base, -1, BEV_OPT_CLOSE_ON_FREE);
		if (!impl_->bev) {
			msg = ("allocate bufferevent failed");
			break;
		}
		bufferevent_setcb(impl_->bev, Impl::readcb, Impl::writecb, Impl::eventcb, this);
		bufferevent_enable(impl_->bev, EV_READ | EV_WRITE);

		if (bufferevent_socket_connect(impl_->bev, (sockaddr*)(&sin), sizeof(sin)) < 0) {
			msg = ("error starting connection");
			break;
		}

		lastTimeSendData = std::chrono::steady_clock::now();

		impl_->thread = std::thread([this]() {
			JLOG_WARN("libevent thread started");
			event_base_dispatch(this->impl_->base);
			JLOG_WARN("libevent thread exited");
		});

		started_ = true;
		return true;
	} while (0);

	stop();
	return false;
}

void Client::stop()
{
	std::lock_guard<std::mutex> lg(mutex_);
	if (!impl_) { return; }

	auto old = autoReconnect_;
	autoReconnect_ = false;

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

void Client::send(const char* data, size_t len)
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
