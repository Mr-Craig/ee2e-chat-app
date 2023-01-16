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
	
	std::thread lookupThread;
	if(argc >= 3) {
		if(std::string(argv[1]) == "--ip") {
			std::string serverName = std::string(argv[2]);
			// Start Ping
			int port = ws->getPort();


			VERBOSE("Main", "Server Name: {}:{}", serverName, port);
			lookupThread = std::thread(utils::pingLookupServer, serverName, port);
		}
	}

	// get server ip from argument
	/*if(argc > 1) {
		serverIp = std::string(argv[1]);
	} */
	
	db::get()->get();
	ws->registerEvent("relay", [](uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		LOG("Socket", "Relay Message: {}", message.get<std::string>());
		ws->send("relayed");
	});

	ws->registerEvent("login", [](uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		
	});

	ws->registerEvent("register", [](uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		
	});

	ws->registerEvent("username_check", [](uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		
	});

	ws->getApp().run();
	
	return 0;
}
