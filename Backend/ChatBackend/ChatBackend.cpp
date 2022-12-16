// ChatBackend.cpp : Defines the entry point for the application.
//

#include "Debug/Debug.h"
#include "Database/Database.h"
#include "Utils/Utils.h"
#include "Socket/Socket.h"
#include "ChatBackend.h"

using namespace std;

int main()
{
	std::shared_ptr<websocket> ws = websocket::get();
	
	db::get()->get();
	ws->registerEvent("relay", [](uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		LOG("Socket", "Relay Message: {}", message.get<std::string>());
		ws->send("relayed");
	});

	ws->getApp().run();

	FATAL("General", "Application closed unexpectedly");
	return 0;
}
