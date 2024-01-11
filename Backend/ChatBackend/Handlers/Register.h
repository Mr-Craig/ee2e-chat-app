#include <string>
#include "../Utils/Argon2.h"
#include <uwebsockets/App.h>	
#include <nlohmann/json.hpp>

namespace Handlers
{
	static void handleRegister(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		if (ws->getUserData()->authed) {
			return;
		}

		Types::UserInfo newUser(message);

		if (!newUser.isValid()) {
			ws->send(Types::BasicResponse("register", "Info provided was invalid.", false).toJson(), uWS::OpCode::TEXT);
			return;
		}

		// Hash Password
		std::string hashedPassword = utils::argon2::hashPassword(newUser.password);

		if (hashedPassword.length() <= 0) {
			// failed to hash password :[	
			ws->send(Types::BasicResponse("register", "Failed to hash password.", false).toJson(), uWS::OpCode::TEXT);
			return;
		}

		newUser.password = hashedPassword;

		if (db::get()->registerUser(newUser)) {
			ws->getUserData()->authed = true;
			ws->getUserData()->username = newUser.username;
			websocket::get()->addUser(newUser.username, ws);
			ws->send(Types::BasicResponse("register", "", true).toJson(), uWS::OpCode::TEXT);
			ws->send(Types::AuthedMessage(newUser.username, notifs::get()->getPublicKey()).toJson(), uWS::OpCode::TEXT);
			return;
		}
		else {
			ws->send(Types::BasicResponse("register", "Failed to insert user into Database.", false).toJson(), uWS::OpCode::TEXT);
			return;
		}
	}
}