#include <iostream>
#include "Alex.h"
#include "Socket.h"


int main(int argc, char** argv)
{
	if (argc != 4) {
		FATAL("Alex", "Not enough arguments supplied! <server> <username> <password>");
		return 1;
	}

	std::string uri = fmt::format("wss://{}", argv[1]);
	std::string username = argv[2];
	std::string password = argv[3];
	LOG("Alex", "Connecting to {}, using {}:{}", uri, username, password);

	Socket* client = new Socket(uri);
	Alex* alex = new Alex(client, username, password);
	client->run();

	return 0;
};