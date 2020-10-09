#include "../../jlib/net/simple_libevent_client.h"
#include "../../jlib/net/simple_libevent_server.h"
#include "../../jlib/net/simple_libevent_clients.h"

using namespace jlib::net;

int main()
{
	simple_libevent_server server;
	simple_libevent_client client;
	simple_libevent_clients clients(nullptr, nullptr, nullptr, nullptr, 1, nullptr);
}
