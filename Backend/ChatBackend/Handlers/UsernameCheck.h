#include <string>
#include <uwebsockets/App.h>	
#include <nlohmann/json.hpp>

namespace Handlers
{
	static void handleCheckUsername(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		if (!message.is_string())
			return;
			
		std::string usernameToCheck = message.get<std::string>();

		bool usernameAvailable = db::get()->checkUsername(usernameToCheck);
		nlohmann::json ret;
		ret["body"] = { {"username", usernameToCheck}, {"available", usernameAvailable} };
		ret["event"] = "username_check";
		ws->send(ret.dump(), uWS::OpCode::TEXT);
	}
}