#include "../../jlib/net/simple_libevent_server.h"
#include "../../jlib/misc/sudoku.h"
#include "../../jlib/log2.h"

using namespace jlib::net;
using namespace jlib::misc::sudoku;

// proto: 89 bytes
// [id:] puzzle \r\n
// n     81   2

Helper helper{};

bool processRequest(simple_libevent_server::BaseClient* client, const std::string& request)
{
	std::string id, puzzle, result, response;

	std::string::const_iterator colon = std::find(request.begin(), request.end(), ':');
	if (colon != request.end()) {
		id.assign(request.cbegin(), colon);
		puzzle.assign(colon + 1, request.cend());
	} else {
		puzzle = request;
	}

	if (puzzle.size() == 81) {
		if (!solve(puzzle, result, &helper)) {
			result = "No solution!";
		} 

		if (id.empty()) {
			response = result + "\r\n";
		} else {
			response = id + ":" + result + "\r\n";
		}

		client->send(response.c_str(), response.size());
		return true;
	} else {
		return false;
	}
}

size_t onMessageCallback(const char* data, size_t len, simple_libevent_server::BaseClient* client, void* user_data)
{
	size_t ate = 0;
	while (len - ate >= 81 + 2) {
		const char* crlf = strstr(data + ate, "\r\n");
		if (crlf) {
			std::string request(data + ate, crlf);
			ate = crlf - data + 2;
			if (!processRequest(client, request)) {
				std::string response("Bad Request!\r\n");
				client->send(response.c_str(), response.size());
				client->shutdown();
				break;
			}
		} else if (len - ate > 100) {
			std::string resp("Id too long!\r\n");
			client->send(resp.c_str(), resp.size());
			client->shutdown();
			break;
		} else {
			break;
		}
	}
	return ate;
}

int main(int argc, char** argv)
{
	int port = 9981;
	if (argc > 1) {
		port = atoi(argv[1]);
	}

	jlib::init_logger(L"sudoku_server");

	simple_libevent_server server;
	server.setThreadNum((int)std::thread::hardware_concurrency());
	server.setOnMsgCallback(onMessageCallback);
	server.setClientMaxIdleTime(100);
	std::string msg;
	if (!server.start(port, msg)) {
		JLOG_CRTC(msg);
		return -1;
	}

	while (1) {
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}
}