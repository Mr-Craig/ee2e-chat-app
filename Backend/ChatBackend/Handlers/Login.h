#include <string>
#include <uwebsockets/App.h>	
#include <nlohmann/json.hpp>

namespace Handlers
{
	static void handleLogin(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		if (ws->getUserData()->authed) {
			// ignore if already authed.
			return;
		}
		Types::UserInfo user(message);

		if (!user.isValid()) {
			ws->send(Types::BasicResponse("login", "Info provided was invalid.", false).toJson(), uWS::OpCode::TEXT);
			return;
		}

		if (db::get()->login(user)) {
			ws->getUserData()->authed = true;
			ws->getUserData()->username = user.username;
			// send login state before sending queued messages.
			ws->send(Types::BasicResponse("login", "", true).toJson(), uWS::OpCode::TEXT);
			ws->send(Types::AuthedMessage(user.username, notifs::get()->getPublicKey()).toJson(), uWS::OpCode::TEXT);
			websocket::get()->addUser(user.username, ws);
			return;
		}
		else {
			ws->send(Types::BasicResponse("login", "Failed to verify details, please try again.", false).toJson(), uWS::OpCode::TEXT);
			return;
		}
	}
}