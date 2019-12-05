
#include "../../jlib/base/config.h"
#include <stdio.h>
#include "../../jlib/net.h"


int main(int argc, char** argv)
{
	const char* domain = "baidu.com";
	int max_sequence_number = 4;

	if (argc > 1) {
		domain = argv[1];
	}

	if (argc > 2) {
		max_sequence_number = std::stoi(argv[2]);
	}

	boost::asio::io_service ios;
	jlib::net::pinger pinger(ios, domain, max_sequence_number, [](const std::string& msg) { printf("%s\n", msg.data()); });
	ios.run();
	printf("average %lldms\n", pinger.get_average());
}
