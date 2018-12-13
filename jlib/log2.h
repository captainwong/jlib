#pragma once

#ifdef JLIB_DISABLE_LOG

namespace jlib {

#define init_logger
#define JLOG_INFO
#define JLOG_WARN
#define JLOG_ERRO
#define JLOG_CRTC
#define JLOG_ALL

class range_log {
public:
	range_log() {}
	range_log(const char*) {}
};

#define AUTO_LOG_FUNCTION

#define dump_hex
#define dump_asc
#define JLOG_HEX
#define JLOG_ASC

}

#else // JLIB_DISABLE_LOG

#define JLIB_LOG2_ENABLED

#include <iostream>
#include "3rdparty/spdlog/spdlog.h"

#ifdef WIN32
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // _CRT_SECURE_NO_WARNINGS
#include "3rdparty/spdlog/sinks/msvc_sink.h"
#include "win32.h"
#endif // WIN32

#include "utf8.h"

namespace jlib {
    
static constexpr char g_logger_name[] = "jlogger";
    
inline void init_logger(const 
#ifdef WIN32
std::wstring
#else
std::string
#endif
& file_name)
{
    try {
		std::vector<spdlog::sink_ptr> sinks;
#ifdef WIN32
		sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
		sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>(
#ifdef WIN32
            file_name + L".log"
#else
            file_name + ".log"
#endif
            , 23, 59));

		auto combined_logger = std::make_shared<spdlog::logger>(g_logger_name, begin(sinks), end(sinks));
		combined_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [tid %t] [%L] %v");
		spdlog::register_logger(combined_logger);
		combined_logger->flush_on(spdlog::level::trace);
	} catch (const spdlog::spdlog_ex& ex) {
#ifdef WIN32
		char msg[1024] = { 0 };
		sprintf_s(msg, "Log initialization failed: %s\n", ex.what());
		MessageBoxA(nullptr, msg, "Error", MB_ICONERROR);
#else
		std::cerr << "Log initialization failed: " << ex.what() << std::endl;
#endif
		exit(0);
	}
    
}


#define JLOG_DBUG spdlog::get(jlib::g_logger_name)->debug
#define JLOG_INFO spdlog::get(jlib::g_logger_name)->info
#define JLOG_WARN spdlog::get(jlib::g_logger_name)->warn
#define JLOG_ERRO spdlog::get(jlib::g_logger_name)->error
#define JLOG_CRTC spdlog::get(jlib::g_logger_name)->critical

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
		JLOG_DBUG(msg_ + " in");
		begin_ = std::chrono::steady_clock::now();
	}

	range_log(const std::wstring& msg) : msg_() {
		msg_ = utf8::w2a(msg);
		JLOG_DBUG(msg_ + " in");
		begin_ = std::chrono::steady_clock::now();
	}

	~range_log() {
		auto diff = std::chrono::steady_clock::now() - begin_;
		auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
		JLOG_DBUG("{} out, duration: {}(ms)", msg_, msec.count());
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

	char output[MAX_OUT_BUFF_LEN] = { 0 };
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

inline void dump_asc(const void* buff, size_t buff_len, bool seperate_with_space = true, bool force_new_line = true)
{
	size_t output_len = buff_len * 6 + 64;
	if (output_len > MAX_OUT_BUFF_LEN) {
		JLOG_ERRO("dump_asc, output_len {} > MAX_OUT_BUFF_LEN {}", output_len, MAX_OUT_BUFF_LEN);
		return;
	}

	char output[MAX_OUT_BUFF_LEN] = { 0 };
	char c[128] = { 0 };
	std::sprintf(c, "dump_asc: buff 0x%p, buff_len %zu\n", buff, buff_len);
	std::strcat(output, c); std::strcat(output, "\n");

	if (force_new_line) {
		for (size_t i = 0; i < 16; i++) {
			char tmp[4];
			std::sprintf(tmp, "%02zX", i);
			std::strcat(c, tmp);
			if (seperate_with_space) {
				std::strcat(c, " ");
			}
		}
		std::strcat(output, c); std::strcat(output, "\n");
	}

	for (size_t i = 0; i < buff_len; i++) {
		std::sprintf(c, "%c", reinterpret_cast<const char*>(buff)[i]);
		if (seperate_with_space) {
			std::strcat(c, "  ");
		}
		std::strcat(output, c);
		if (force_new_line && i > 0 && (i + 1) % 16 == 0) {
			std::strcat(output, "\n");
		}
	}

	JLOG_WARN(output);
}

#define JLOG_HEX(b, l) jlib::dump_hex(b, l)
#define JLOG_ASC(b, l) jlib::dump_asc(b, l)

}

#endif // end of JLIB_DISABLE_LOG
