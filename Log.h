#pragma once

#ifdef WIN32
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
//#include <Windows.h>
#include "win32.h"
#endif

// uncomment line below to disable OutputDebugString
// #define NO_WINDOWS

#include <assert.h>
#include <mutex>
#include <fstream>
#include <string>
#include <memory>
#include <cstring>
#include <cstdint>
#include <cstdarg>
#include <algorithm>
#include <boost/noncopyable.hpp>
#include "utf8.h"
#include "chrono_wrapper.h"
#include "dp/singleton.h"

namespace jlib
{

#define JLOG jlib::log::log_utf16
#define JLOGA jlib::log::log_utf8
#define JLOGW jlib::log::log_utf16
#define JLOGB(b, l) jlib::log::dump_hex(b, l)
#define JLOGASC(b, l) jlib::log::dump_ascii(b, l)

class log : public dp::singleton<log>
{
	enum { max_output_size = 1024 * 64, max_single_log_file_size = 1024 * 1024 * 10 };

private:
	bool log_to_file_ = false;
	bool log_to_dbg_view_ = true;
	bool log_to_console_ = true;
	bool running_ = false;
	std::ofstream log_file_;
	std::string log_file_foler_ = "";
	std::string log_file_path_ = "";
	std::string line_prefix_ = "";
	std::string log_file_prefix_ = "";
	std::mutex lock_;

public:
	// initializers, they should be called right after get_instance
	void set_line_prifix(const std::string& prefix) { line_prefix_ = prefix; }

	void set_output_to_dbg_view(bool b = true) { log_to_dbg_view_ = b; }
	
	void set_output_to_console(bool b = true) { log_to_console_ = b; }

	// 3 functions below must be called by sequence
	void set_log_file_foler(const std::string& folder_path) { log_file_foler_ = folder_path.empty() ? "" : folder_path + "\\"; }
	void set_log_file_prefix(const std::string& prefix) { log_file_prefix_ = prefix; }
	void set_output_to_file(bool b = true) { log_to_file_ = b; if (b) create_file_name(); }

	std::string get_log_file_path() const { return log_file_path_; }

public:
	
	~log()
	{
		running_ = false;

		if (log_file_.is_open()) {
			log_file_.close();
		}

#ifdef _DEBUG
		output_to_dbg_view("log::~log\n");
#endif
	}

public:

	// static operations

	static void dump_hex(const char* buff, size_t buff_len)
	{
		try {
			auto instance = log::get_instance();

			if (instance->log_to_file_ || instance->log_to_dbg_view_ || instance->log_to_console_) {
				size_t output_len = buff_len * 6 + 64;
				std::unique_ptr<char[]> output = std::unique_ptr<char[]>(new char[output_len]);
				output[0] = 0;
				char c[64] = { 0 };
				std::sprintf(c, "len %d\n", buff_len);
				std::strcat(output.get(), c);
				for (size_t i = 0; i < buff_len; i++) {
					std::sprintf(c, "%02X ", static_cast<unsigned char>(buff[i]));
					std::strcat(output.get(), c);
					if (i > 0 && (i + 1) % 16 == 0) {
						std::strcat(output.get(), "\n");
					}
				}
				std::strcat(output.get(), "\n");
				instance->output(output.get());
			}
		} catch (...) {
			assert(0);
		}
	}

	static void dump_ascii(const char* buff, size_t buff_len)
	{
		try {
			auto instance = log::get_instance();

			if (instance->log_to_file_ || instance->log_to_dbg_view_ || instance->log_to_console_) {
				size_t output_len = buff_len * 6 + 64;
				std::unique_ptr<char[]> output = std::unique_ptr<char[]>(new char[output_len]);
				output[0] = 0;
				char c[64] = { 0 };
				std::sprintf(c, "len %d\n", buff_len);
				std::strcat(output.get(), c);
				for (size_t i = 0; i < buff_len; i++) {
					std::sprintf(c, "%C ", static_cast<unsigned char>(buff[i]));
					std::strcat(output.get(), c);
					if (i > 0 && (i + 1) % 16 == 0) {
						std::strcat(output.get(), "\n");
					}
				}
				std::strcat(output.get(), "\n");
				instance->output(output.get());
			}
		} catch (...) {
			assert(0);
		}
	}

	static void log_utf16(const wchar_t* format, ...)
	{
		try {
			auto instance = log::get_instance();

			if (instance->log_to_file_ || instance->log_to_dbg_view_ || instance->log_to_console_) {
				wchar_t buf[max_output_size], *p;
				p = buf;
				va_list args;
				va_start(args, format);
				p += _vsnwprintf_s(p, max_output_size - 1, max_output_size - 1, format, args);
				va_end(args);
				while ((p > buf) && (*(p - 1) == '\r' || *(p - 1) == '\n'))
					*--p = '\0';
				*p++ = '\r';
				*p++ = '\n';
				*p = '\0';

#if defined(WIN32)
				auto msg = instance->format_msg(utf8::u16_to_mbcs(buf));
#else
				auto msg = instance->format_msg(utf8::w2a(buf));
#endif
				instance->output(msg);
			}
		} catch (...) {
			assert(0);
		}
	}

	static void log_utf8(const char* format, ...)
	{
		try {
			auto instance = log::get_instance();

			if (instance->log_to_file_ || instance->log_to_dbg_view_ || instance->log_to_console_) {
				char buf[max_output_size], *p;
				p = buf;
				va_list args;
				va_start(args, format);
				p += _vsnprintf_s(p, max_output_size - 1, max_output_size - 1, format, args);
				va_end(args);
				while ((p > buf) && (*(p - 1) == '\r' || *(p - 1) == '\n'))
					*--p = '\0';
				*p++ = '\r';
				*p++ = '\n';
				*p = '\0';

				instance->output(instance->format_msg(buf));
			}
		} catch (...) {
			assert(0);
		}
	}


protected:

	log() {
		running_ = true;
		char out[128] = { 0 };
		sprintf(out, "log construction addr: %p\n", this);
		output_to_dbg_view(out);
	}

	bool open_log_file() {
		if (!log_file_.is_open()) {
			log_file_.open(log_file_path_);
		}
		return log_file_.is_open();
	}

	void create_file_name() {
		auto s = now_to_string();
		std::replace(s.begin(), s.end(), ' ', '_');
		std::replace(s.begin(), s.end(), ':', '-');
		log_file_path_ = log_file_foler_ + log_file_prefix_ + "." + s + ".log";
	}

	std::string format_msg(const std::string& msg) {
		return line_prefix_ + " " + now_to_string(true) + " ---- " + msg;
	}

	void output(const std::string& msg) {
		std::lock_guard<std::mutex> lock(lock_);
		if (log_to_file_) {
			output_to_log_file(msg);
		}

		if (log_to_dbg_view_) {
			output_to_dbg_view(msg);
		}

		if (log_to_console_) {
			std::printf(msg.c_str());
		}
	}

	void output_to_log_file(const std::string& msg) {
		if (log_file_.is_open()) {
			auto size = log_file_.tellp();
			if (size > max_single_log_file_size) {
				log_file_.close();
				create_file_name();
				open_log_file();
			}
		}

		if (!log_file_.is_open()) {
			open_log_file();
		}

		if (log_file_.is_open()) {
			log_file_.write(msg.c_str(), msg.size());
			log_file_.flush();
		}
	}

	void output_to_dbg_view(const std::string& msg) {
#if defined(WIN32) && !defined(NO_WINDOWS)
		OutputDebugStringA(msg.c_str());
#else
		std::printf(msg.c_str());
#endif
	}

	

};


class range_log
{
private:
	std::string msg_;
	std::chrono::steady_clock::time_point begin_;

public:
	range_log(const std::string& msg) : msg_(msg) {
		JLOGA((msg_ + " in").c_str());
		begin_ = std::chrono::steady_clock::now();
	}

	range_log(const std::wstring& msg) : msg_() {
		msg_ = utf8::w2a(msg);
		JLOGA((msg_ + " in").c_str());
		begin_ = std::chrono::steady_clock::now();
	}

	~range_log() {
		auto diff = std::chrono::steady_clock::now() - begin_;
		auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
		JLOGA("%s out, duration: %d(ms)\n", msg_.c_str(), msec.count());
	}
};

#define AUTO_LOG_FUNCTION jlib::range_log __log_function_object__(__FUNCTION__);





};
