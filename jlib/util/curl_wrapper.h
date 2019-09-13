#pragma once

/**
* simple curl wrapper
* under windows, you should config project's include/lib path to your curl-path.
* define CURL_STATICLIB in your prject
* the input libs are: libcurl.lib;ws2_32.lib;wldap32.lib;crypt32.lib;
*/


#include <curl/curl.h>
#include <string>
#include "mem_tool.h"

namespace jlib
{

struct CurlHelper {
	CurlHelper() {
		curl_global_init(CURL_GLOBAL_ALL);
	}

	~CurlHelper() {
		curl_global_cleanup();
	}
};

struct Curl
{
	Curl() { static CurlHelper helper = {}; }

	~Curl() {
		curl_easy_cleanup(curl_);
	}

	bool init() {
		curl_ = curl_easy_init();
		curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, Curl::write_data);
		curl_easy_setopt(curl_, CURLOPT_WRITEDATA, this);
		curl_easy_setopt(curl_, CURLOPT_HEADER, 1);
		curl_easy_setopt(curl_, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 0);
		return curl_ != nullptr;
	}

	operator CURL* () {
		return curl_;
	}

	CURLcode lastError() const { return lastErorrCode_; }
	std::string lastErrorMsg() const { return lastErrorMsg_; }
	long lastHttpCode() const { return lastHttpCode_; }
	std::string lastHttpContent() const { return lastHttpContent_; }

	bool get(const std::string& url, int timeout = 10) {
		curl_easy_setopt(curl_, CURLOPT_URL, url.data());
		curl_easy_setopt(curl_, CURLOPT_TIMEOUT, timeout);
		return perform();
	}

	bool post(const std::string& url, const std::string& data, int timeout = 10) {
		curl_easy_setopt(curl_, CURLOPT_URL, url.data());
		curl_easy_setopt(curl_, CURLOPT_TIMEOUT, timeout);
		curl_easy_setopt(curl_, CURLOPT_POST, 1);
		curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data.data());
		return perform();
	}

	bool postJson(const std::string& url, const std::string& json, int timeout = 10) {
		curl_easy_setopt(curl_, CURLOPT_URL, url.data());
		curl_easy_setopt(curl_, CURLOPT_TIMEOUT, timeout);
		curl_easy_setopt(curl_, CURLOPT_POST, 1);
		curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, json.data());

		curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, "application/json");
		curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);
		auto_free<curl_slist> auto_free_headers(headers, [](curl_slist* p) { curl_slist_free_all(p); });

#ifdef JLIB_LOG2_ENABLED
		dump_slist(headers);
#endif // JLIB_LOG2_ENABLED

		return perform();
	}

#ifdef JLIB_LOG2_ENABLED
	static void dump_slist(curl_slist* list) {
		JLOG_INFO("dumping curl_slist:");
		while (list) {
			JLOG_INFO("    {:x}", list->data);
			list = list->next;
		}
	}
#endif // JLIB_LOG2_ENABLED


///////////////////////////// details ///////////////////////////////

	CURL* curl_ = nullptr;
	std::string buffer_ = {};
	CURLcode lastErorrCode_ = CURLcode::CURLE_OK;
	std::string lastErrorMsg_ = {};
	long lastHttpCode_ = 0;
	std::string lastHttpContent_ = {};

	static size_t write_data(void* buffer, size_t size, size_t nmemb, void* user) {
		auto curl = reinterpret_cast<Curl*>(user);
		curl->buffer_.append(reinterpret_cast<const char*>(buffer), size * nmemb);
		return size * nmemb;
	}

	bool parse_centent() {
		long header_size = 0;
		curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &lastHttpCode_);
		curl_easy_getinfo(curl_, CURLINFO_HEADER_SIZE, &header_size);
		if (header_size < 0) {
#ifdef JLIB_LOG2_ENABLED
			JLOG_ERRO("get responce headers error");
#endif // JLIB_LOG2_ENABLED
			return false;
		}
		auto sheaders = buffer_.substr(0, header_size);
		auto shttp_code = std::to_string(lastHttpCode_);
		auto code_pos = sheaders.find(shttp_code);
		auto end_pos = sheaders.find("\r\n");
		auto content_length = buffer_.size() - sheaders.size();
		lastHttpContent_ = (buffer_.substr(sheaders.size()));
#ifdef JLIB_LOG2_ENABLED
		JLOG_INFO("responce status: {}", lastHttpCode_);
		JLOG_INFO("responce body:\n{}", lastHttpContent_);
#endif // JLIB_LOG2_ENABLED
		return true;
	}

	bool perform() {
		auto ret = curl_easy_perform(curl_);
		if (ret != CURLE_OK) {
			lastErorrCode_ = ret;
			lastErrorMsg_ = curl_easy_strerror(ret);
#ifdef JLIB_LOG2_ENABLED
			JLOG_ERRO("curl_easy_perform error {}:{}", lastHttpCode_, lastErrorMsg_);
#endif // JLIB_LOG2_ENABLED
			return false;
		}
		return parse_centent();
	}

};

}
