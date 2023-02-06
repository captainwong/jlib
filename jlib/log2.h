#pragma once

#include "base/config.h"

#ifdef JLIB_DISABLE_LOG
#include "log2micros.h"
#else // JLIB_DISABLE_LOG

#define JLIB_LOG2_ENABLED

#ifdef JLIB_WINDOWS
#define SPDLOG_WCHAR_FILENAMES
#include <Windows.h>
#endif // JLIB_WINDOWS

#include <stdio.h>

#define SPDLOG_HEADER_ONLY
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>

#ifdef JLIB_WINDOWS
#include <spdlog/sinks/msvc_sink.h>
#endif // JLIB_WINDOWS

namespace jlib {
    
static constexpr char g_logger_name[] = "jlogger";
    
inline void init_logger( 
#ifdef JLIB_WINDOWS
std::wstring file_name = L""
#else
std::string file_name = ""
#endif
)
{
	if (!file_name.empty()) {
#ifdef JLIB_WINDOWS
		file_name += L".log";
#else
		file_name += ".log";
#endif
	}

    try {
		std::vector<std::shared_ptr<spdlog::sinks::sink>> sinks;
#ifdef JLIB_WINDOWS
		sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif
		sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>());
		if (!file_name.empty()) {
			sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>(file_name, 23, 59));
		}
		auto combined_logger = std::make_shared<spdlog::logger>(g_logger_name, begin(sinks), end(sinks));
		combined_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%L] %v");
		spdlog::register_logger(combined_logger);
		combined_logger->flush_on(spdlog::level::trace);
	} catch (const spdlog::spdlog_ex& ex) {
#ifdef JLIB_WINDOWS
		char msg[1024] = { 0 };
		sprintf_s(msg, "Log initialization failed: %s\n", ex.what());
		MessageBoxA(nullptr, msg, "Error", MB_ICONERROR);
#else
		fprintf(stderr, "Log initialization failed: %s\n", ex.what());
#endif
		exit(0);
	}    
}


#define JLOG_DBUG spdlog::get(jlib::g_logger_name)->debug
#define JLOG_INFO spdlog::get(jlib::g_logger_name)->info
#define JLOG_WARN spdlog::get(jlib::g_logger_name)->warn
#define JLOG_ERRO spdlog::get(jlib::g_logger_name)->error
#define JLOG_CRTC spdlog::get(jlib::g_logger_name)->critical

#ifdef JLIB_WINDOWS
#define JLOG_ALL(args, ...) spdlog::get(jlib::g_logger_name)->log(spdlog::level::off, args, __VA_ARGS__)
#else
#define JLOG_ALL(args...) spdlog::get(jlib::g_logger_name)->log(spdlog::level::off, args)
#endif /* JLIB_WINDOWS */

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

	~range_log() {
		auto diff = std::chrono::steady_clock::now() - begin_;
		auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
		JLOG_DBUG("{} out, duration: {}(ms)", msg_, msec.count());
	}
};

#ifdef JLIB_WINDOWS
#define __the_pretty_name_of_this_function__ __FUNCTION__
#else
#define __the_pretty_name_of_this_function__ __PRETTY_FUNCTION__
#endif

#define AUTO_LOG_FUNCTION jlib::range_log __log_function_object__(__the_pretty_name_of_this_function__);


constexpr size_t MAX_OUT_BUFF_LEN = 4096;
constexpr size_t MAX_INPUT_BUFF_LEN = 672;

inline void dump_hex(const void* buff, size_t buff_len, spdlog::level::level_enum level_enum = spdlog::level::warn)
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

	spdlog::get(jlib::g_logger_name)->log(level_enum, output);
}

inline void dump_asc(const void* buff, size_t buff_len, bool seperate_with_space = true, bool force_new_line = true, spdlog::level::level_enum level_enum = spdlog::level::warn)
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
		if (reinterpret_cast<const unsigned char*>(buff)[i] < 128 && std::isprint(reinterpret_cast<const char*>(buff)[i])) {
			std::sprintf(c, "%c", reinterpret_cast<const char*>(buff)[i]);
		} else {
			std::sprintf(c, "$");
		}
		
		if (seperate_with_space) {
			std::strcat(c, "  ");
		}
		std::strcat(output, c);
		if (force_new_line && i > 0 && (i + 1) % 16 == 0) {
			std::strcat(output, "\n");
		}
	}

	spdlog::get(jlib::g_logger_name)->log(level_enum, output);
}

#define JLOG_HEX(b, l) jlib::dump_hex((b), (l))
#define JLOG_ASC(b, l) jlib::dump_asc((b), (l))

}

#endif // end of JLIB_DISABLE_LOG
