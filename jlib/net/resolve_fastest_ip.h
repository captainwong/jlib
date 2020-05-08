#pragma once

#include "../log2auto.h"
#include "ping.h"

namespace jlib {
namespace net {

using waiting_cb = std::function<void(void)>;

namespace detail {

static std::pair<long long, std::string> pingIpGetAverageMs(const std::string& ip, int ping_times, jlib::net::pinger::Output output)
{
	boost::asio::io_service io_service;
	JLOG_INFO("ping ip:{}", ip);
	jlib::net::pinger p(io_service, ip.c_str(), ping_times, output);
	io_service.run();
	auto ms = p.get_average();
	JLOG_INFO("ip:{} 's average delay is {}ms", ip, ms);
	return { ms, ip };
}

static std::pair<bool, std::string> get_domain_ip_impl(const std::string& domain, int ping_times) {
	AUTO_LOG_FUNCTION;
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query(domain, "");

	try {
		std::string fastest_ip;
		std::vector<std::string> ips;
		long long fastest_ping_ms = 500000000;
		JLOG_INFO("resolving domain:{}", domain);
		auto iter = resolver.resolve(query);
		boost::asio::ip::tcp::resolver::iterator end;
		while (iter != end) {
			boost::asio::ip::tcp::endpoint endpoint = *iter++;
			std::string ip = endpoint.address().to_string();
			ips.push_back(ip);
		}

		std::vector<std::future<std::pair<long long, std::string>>> threads;
		for (auto ip : ips) {
			threads.emplace_back(std::async(std::launch::async, pingIpGetAverageMs, ip, ping_times, [](const std::string& msg) {
				JLOG_INFO(msg);
			}));
		}

		bool allDone = true;
		while (!allDone) {
			allDone = true;
			for (auto& f : threads) {
				auto status = f.wait_for(std::chrono::milliseconds(1));
				if (status != std::future_status::ready) {
					allDone = false;
				}
			}
		}

		for (auto& f : threads) {
			auto res = f.get();
			if (res.first < fastest_ping_ms) {
				fastest_ping_ms = res.first;
				fastest_ip = res.second;
			}
		}

		JLOG_INFO("fastest ip of domain:{} is {}", domain, fastest_ip);
		return std::pair<bool, std::string>(true, fastest_ip);
	} catch (std::exception& e) {
		return std::pair<bool, std::string>(false, e.what());
	}
}

} // end of namespace detail


static bool get_domain_ip(const std::string& domain, int ping_times, std::string& result, waiting_cb cb) {
	auto f = std::async(std::launch::async, detail::get_domain_ip_impl, domain, ping_times);
	while (true) {
		auto status = f.wait_for(std::chrono::milliseconds(1));
		if (status == std::future_status::ready) {
			break;
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			if (cb) { cb(); }
		}
	}

	auto ret = f.get();
	result = ret.second;
	return ret.first;
}

}
}
