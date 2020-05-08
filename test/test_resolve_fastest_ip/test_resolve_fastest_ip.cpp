#include "../../jlib/log2.h"
#include "../../jlib/net/resolve_fastest_ip.h"


int main(int argc, char** argv)
{
	std::string domain = "baidu.com";
	int ping_times = 4;
	if (argc > 1) { domain = argv[1]; }
	if (argc > 2) { ping_times = std::stoi(argv[2]); }
	if (argc < 1) { printf("Usage: %s [domain] [ping times]\n", argv[0]); return 1; }

	jlib::init_logger();

#ifndef _DEBUG
	spdlog::get(jlib::g_logger_name)->set_level(spdlog::level::err);
#endif

	std::string res;
	int n = 0;
	bool ret = jlib::net::get_domain_ip(domain, ping_times, res, [&n]() {
		if (++n == 4) { n = 0; }
		std::string msg = "waiting";
		for (int i = 0; i < n; i++) { msg.push_back('.'); }
		JLOG_INFO(msg);
	});

	if (ret) {
		JLOG_INFO("get_domain_ip of [{}] ok: {}", domain, res);
		printf("%s\n", res.data());
		return 0;
	} else {
		JLOG_ERRO("get_domain_ip failed: {}", res);
		printf("");
		return 1;
	}
}
