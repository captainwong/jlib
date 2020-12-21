#include "../../jlib/net/simple_libevent_clients.h"
#include "../../jlib/misc/sudoku.h"
#include "../../jlib/log2.h"
#include "../../jlib/util/rand.h"

using namespace jlib::net;
using namespace jlib::misc::sudoku;

// proto: 89 bytes
// [id:] puzzle \r\n
// n     81   2

const char* ip = "127.0.0.1";
int port = 9981;
int thread_count = 10;
int session_count = 10;
int session_connected = 0;
int session_disconnected = 0;
int puzzles_to_solve_per_client = 50;
bool done = false;
std::mutex mutex{};


struct Client : simple_libevent_clients::BaseClient {
	Helper helper{};
	int puzzles = 1000;
	int npuzzle = 0;

	Client() : BaseClient() {}

	static BaseClient* createClient()
	{
		std::lock_guard<std::mutex> lg(mutex);
		auto client = new Client();
		client->puzzles = puzzles_to_solve_per_client;
		//client->helper.rng = jlib::seeded_random_engine();
		return client;
	}

	static void onConn(bool up, const std::string& msg, BaseClient* client_, void* user_data)
	{
		auto client = (Client*)client_;

		if (up) {
			client->sendPuzzle();

			{
				std::lock_guard<std::mutex> lg(mutex);
				printf("live connections %d\n", ++session_connected);
				if (session_connected == session_count) {
					printf("All connected\n");
					return;
				}
			}

			jlib::net::simple_libevent_clients* clients = (jlib::net::simple_libevent_clients*)user_data;
			std::string msg;
			clients->connect(ip, port, msg);

		} else {			

			std::lock_guard<std::mutex> lg(mutex);
			if (++session_disconnected == session_count) {
				done = true;
				printf("All disconnected\n");				
			}
		}
	}

	static size_t onMsg(const char* data, size_t len, jlib::net::simple_libevent_clients::BaseClient* client_, void* user_data)
	{
		auto client = (Client*)client_;

		size_t ate = 0;
		while (len - ate >= 81 + 2) {
			const char* crlf = strstr(data + ate, "\r\n");
			if (crlf) {
				std::string response(data + ate, crlf);
				ate = crlf - data + 2;
				if (!client->processResponse(response)) {
					client->shutdown();
					//done = true;
					break;
				}
			} else if (len - ate > 100) {
				client->shutdown();
				//done = true;
				break;
			} else {
				break;
			}
		}
		return ate;
	}

	void sendPuzzle()
	{
		if (npuzzle++ < puzzles) {
			auto puzzle = random_puzzle(&helper);
			std::string request = std::to_string(npuzzle) + ":" + puzzle;
			JLOG_INFO("T#" + std::to_string(thread_id()) + " " + request);
			request += "\r\n";
			send(request.c_str(), request.size());
		} else {
			shutdown();
		}
	}

	bool processResponse(const std::string& response)
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
			JLOG_INFO("T#" + std::to_string(thread_id()) + " " + info);

			sendPuzzle();
			return true;
		} else {
			JLOG_ERRO(response);
			return false;
		}
	}
};

int main(int argc, char** argv)
{
	if (argc > 1) {
		ip = argv[1];
	}

	if (argc > 2) {
		port = atoi(argv[2]);
	}

	if (argc > 3) {
		thread_count = atoi(argv[3]);
	}

	if (argc > 4) {
		session_count = atoi(argv[4]);
	}
	
	if (argc > 5) {
		puzzles_to_solve_per_client = atoi(argv[5]);
	}

	jlib::init_logger(L"sudoku_clients");

	simple_libevent_clients clients(Client::onConn, Client::onMsg, nullptr, Client::createClient, thread_count, nullptr);
	clients.setUserData(&clients);
	std::string msg;
	if (!clients.connect(ip, port, msg)) {
		JLOG_CRTC(msg);
		return -1;
	}

	while (!done) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

}