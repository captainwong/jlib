#include "../../jlib/net/simple_libevent_client.h"
#include "../../jlib/misc/sudoku.h"
#include "../../jlib/log2.h"

using namespace jlib::net;
using namespace jlib::misc::sudoku;

// proto: 89 bytes
// [id:] puzzle \r\n
// n     81   2

Helper helper{};
int puzzles = 1000;
int npuzzle = 0;
bool done = false;

void sendPuzzle(simple_libevent_client* client)
{
	if (npuzzle++ < puzzles) {
		auto puzzle = random_puzzle(&helper);
		std::string request = std::to_string(npuzzle) + ":" + puzzle;
		JLOG_INFO(request);
		request += "\r\n";
		client->send(request.c_str(), request.size());
	} else {
		//client->stop();
		done = true;
	}
}

void onConnectinoCallback(bool up, const std::string& msg, void* user_data)
{
	if (up) {
		sendPuzzle(reinterpret_cast<simple_libevent_client*>(user_data));
	}
}

bool processResponse(simple_libevent_client* client, const std::string& response)
{
	std::string id, result, info;

	std::string::const_iterator colon = std::find(response.begin(), response.end(), ':');
	if (colon != response.end()) {
		id.assign(response.cbegin(), colon);
		result.assign(colon + 1, response.cend());
	} else {
		result = response;
	}

	if (result.size() == 81) {
		if (id.empty()) {
			info = result;
		} else {
			info = id + ":" + result;
		}
		JLOG_INFO(info);

		sendPuzzle(client);
		return true;
	} else {
		JLOG_ERRO(response);
		return false;
	}
}

size_t onMessageCallback(const char* data, size_t len, void* user_data)
{
	auto client = reinterpret_cast<simple_libevent_client*>(user_data);
	size_t ate = 0;
	while (len - ate >= 81 + 2) {
		const char* crlf = strstr(data + ate, "\r\n");
		if (crlf) {
			std::string response(data + ate, crlf);
			ate = crlf - data + 2;
			if (!processResponse(client, response)) {				
				//client->stop();
				done = true;
				break;
			}
		} else if (len - ate > 100) {
			//client->stop();
			done = true;
			break;
		} else {
			break;
		}
	}
	return ate;
}


int main(int argc, char** argv)
{
	const char* ip = "127.0.0.1";
	int port = 9981;

	if (argc > 1) {
		ip = argv[1];
	}

	if (argc > 2) {
		port = atoi(argv[2]);
	}

	if (argc > 3) {
		puzzles = atoi(argv[3]);
	}

	jlib::init_logger(L"sudoku_server");

	simple_libevent_client client;
	client.setUserData(&client);
	client.setOnConnectionCallback(onConnectinoCallback);
	client.setOnMsgCallback(onMessageCallback);
	std::string msg;
	if (!client.start(ip, port, msg, true)) {
		JLOG_CRTC(msg);
		return -1;
	}

	while (!done) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

}