#pragma once
#include <iostream>
#include "spdlog/spdlog.h"
#ifdef WIN32
#include "spdlog/sinks/msvc_sink.h"
#include "win32.h"
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
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
			utf8::mbcs_to_u16(file_name + ".log")
#else		
			file_name + ".log"
#endif
			, 23, 59));
		auto combined_logger = std::make_shared<spdlog::logger>(g_logger_name, begin(sinks), end(sinks));
		combined_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [tid %t] [%L] %v");
		spdlog::register_logger(combined_logger);
	} catch (const spdlog::spdlog_ex& ex) {
#ifdef WIN32
		char msg[1024] = { 0 };
		sprintf_s(msg, "Log initialization failed: %s\n", ex.what());
		OutputDebugStringA(msg);
#else
		std::cerr << "Log initialization failed: " << ex.what() << std::endl;
#endif
	}
    
}



#define JLOG_INFO spdlog::get(g_logger_name)->info
#define JLOG_WARN spdlog::get(g_logger_name)->warn
#define JLOG_ERRO spdlog::get(g_logger_name)->error
#define JLOG_CRTC spdlog::get(g_logger_name)->critical

#ifdef _WIN32
#define JLOG_ALL(args, ...) spdlog::get(jlib::g_logger_name)->log(spdlog::level::off, args, __VA_ARGS__)
#else
#define JLOG_ALL(args...) spdlog::get(jlib::g_logger_name)->log(spdlog::level::off, args)
#endif /* _WIN32 */

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

#ifdef _WIN32
#define __the_pretty_name_of_this_function__ __FUNCTION__
#else
#define __the_pretty_name_of_this_function__ __PRETTY_FUNCTION__
#endif

#define AUTO_LOG_FUNCTION jlib::range_log __log_function_object__(__the_pretty_name_of_this_function__);


static const int MAX_OUT_BUFF_LEN = 4096;

inline void dump_hex(const void* buff, size_t buff_len)
{
	size_t output_len = buff_len * 6 + 64;
	if (output_len > MAX_OUT_BUFF_LEN) {
		JLOG_ERRO("dump_hex, output_len {} > MAX_OUT_BUFF_LEN {}", output_len, MAX_OUT_BUFF_LEN);
		return;
	}

	char output[4096] = { 0 };
	char c[128] = { 0 };
	std::sprintf(c, "dump_hex: buff 0x%p, buff_len %zu\n", buff, buff_len);

	for (size_t i = 0; i < 16; i++) {
		char tmp[4];
		std::sprintf(tmp, "%02zX ", i);
		std::strcat(c, tmp);
	}

	std::strcat(output, c); std::strcat(output, "\n");

	for (size_t i = 0; i < buff_len; i++) {
		std::sprintf(c, "%02X ", reinterpret_cast<const unsigned char*>(buff)[i]);
		std::strcat(output, c);
		if (i > 0 && (i + 1) % 16 == 0) {
			std::strcat(output, "\n");
		}
	}

	JLOG_WARN(output);
}

inline void dump_asc(const void* buff, size_t buff_len)
{
	size_t output_len = buff_len * 6 + 64;
	if (output_len > MAX_OUT_BUFF_LEN) {
		JLOG_ERRO("dump_asc, output_len {} > MAX_OUT_BUFF_LEN {}", output_len, MAX_OUT_BUFF_LEN);
		return;
	}

	char output[4096] = { 0 };
	char c[128] = { 0 };
	std::sprintf(c, "dump_asc: buff 0x%p, buff_len %zu\n", buff, buff_len);

	for (size_t i = 0; i < 16; i++) {
		char tmp[4];
		std::sprintf(tmp, "%02zX ", i);
		std::strcat(c, tmp);
	}

	std::strcat(output, c); std::strcat(output, "\n");

	for (size_t i = 0; i < buff_len; i++) {
		std::sprintf(c, "%c  ", reinterpret_cast<const char*>(buff)[i]);
		std::strcat(output, c);
		if (i > 0 && (i + 1) % 16 == 0) {
			std::strcat(output, "\n");
		}
	}

	JLOG_WARN(output);
}

#define JLOG_HEX(b, l) jlib::dump_hex(b, l)
#define JLOG_ASC(b, l) jlib::dump_asc(b, l)
}
