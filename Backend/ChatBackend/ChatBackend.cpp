// ChatBackend.cpp : Defines the entry point for the application.
//

#include "Debug/Debug.h"
#include "Database/Database.h"
#include "Utils/Utils.h"
#include "Socket/Socket.h"
#include "ChatBackend.h"

using namespace std;

int main(int argc, char** argv)
{
	std::shared_ptr<websocket> ws = websocket::get();

	// get server ip from argument
	std::string serverIp = "N/A";
	if(argc > 1) {
		serverIp = std::string(argv[1]);
	}

	VERBOSE("Socket", "Server IP: {}", serverIp);
	
	db::get()->get();
	ws->registerEvent("relay", [](uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		LOG("Socket", "Relay Message: {}", message.get<std::string>());
		ws->send("relayed");
	});

	ws->getApp().run();

	FATAL("General", "Application closed unexpectedly");
	return 0;
}
