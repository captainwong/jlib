#pragma once
#include <iostream>
#include <spdlog/spdlog.h>
#ifdef WIN32
#include <spdlog/sinks/msvc_sink.h>
#endif
#include "utf8.h"

namespace jlib{
    
static const char g_logger_name[] = "logger";
    
inline void init_logger(const std::string& file_name = "")
{
    try {
		std::vector<spdlog::sink_ptr> sinks;
#ifdef WIN32
		sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
		sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>(
#ifdef WIN32
			utf8::a2w(file_name + ".log")
#else		
			file_name + ".log"
#endif
			, 23, 59));
		auto combined_logger = std::make_shared<spdlog::logger>(g_logger_name, begin(sinks), end(sinks));
		combined_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [tid %t] [%L] %v");
		spdlog::register_logger(combined_logger);
	} catch (const spdlog::spdlog_ex& ex) {
		std::cerr << "Log initialization failed: " << ex.what() << std::endl;
	}
    
}

#define JLOG_INFO spdlog::get(g_logger_name)->info
#define JLOG_WARN spdlog::get(g_logger_name)->warn
#define JLOG_ERRO spdlog::get(g_logger_name)->error
#define JLOG_CRTC spdlog::get(g_logger_name)->critical

//#define JLOG JLOG_INFO
#define JLOGB(b, l) (b, l)
#define JLOGASC(b, l) (b, l)


class range_log
{
private:
	std::string msg_;
	std::chrono::steady_clock::time_point begin_;

public:
	range_log(const std::string& msg) : msg_(msg) {
		JLOG_INFO(msg_ + " in");
		begin_ = std::chrono::steady_clock::now();
	}

	range_log(const std::wstring& msg) : msg_() {
		msg_ = utf8::w2a(msg);
		JLOG_INFO(msg_ + " in");
		begin_ = std::chrono::steady_clock::now();
	}

	~range_log() {
		auto diff = std::chrono::steady_clock::now() - begin_;
		auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
		JLOG_INFO("{} out, duration: {}(ms)", msg_, msec.count());
	}
};

#define AUTO_LOG_FUNCTION jlib::range_log __log_function_object__(__func__);
    
}
