// ChatBackend.cpp : Defines the entry point for the application.
//

#include "Debug/Debug.h"
#include "Utils/Utils.h"
#include "Socket/Socket.h"
#include "ChatBackend.h"

using namespace std;

int main()
{
	std::shared_ptr<websocket> ws = websocket::get();

	ws->registerEvent("relay", [](uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		LOG("Socket", "Relay Message: {}", message.get<std::string>());
		ws->send("relayed");
	});

	while (ws->isRunning()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	return 0;
}
