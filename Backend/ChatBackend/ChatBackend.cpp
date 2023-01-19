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
	
	db::get()->get();
	ws->registerEvent("relay", [](uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		if(ws->getUserData()->authed && (message.contains("username") && message["username"].is_string()) && (message.contains("body") && message["body"].is_object())) {
			std::string username = message["username"].get<std::string>();
			websocket::get()->relayMessage(username, message["body"]);
		} else {
			WARNING("Relay", "Got relay message but its invalid! {}", message.dump());
		}
	});

	ws->registerEvent("whoami", [](uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		ws->send(ws->getUserData()->username, uWS::OpCode::TEXT);
	});

	ws->registerEvent("login", [](uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		if(ws->getUserData()->authed) {
			// ignore if already authed.
			return;
		}
		Types::UserInfo user(message);

		if(!user.isValid()) {
			nlohmann::json ret;
			ret["body"] = {{"success", false}, {"message", "Info provided was invalid."}};
			ret["event"] = "login";
			
			ws->send(ret.dump(), uWS::OpCode::TEXT);
			return;
		}

		if(db::get()->login(user)) {
			nlohmann::json ret;
			ret["body"] = {{"success", true}, {"message", ""}};
			ret["event"] = "login";
			
			ws->getUserData()->authed = true;
			ws->getUserData()->username = user.username;
			// send login state before sending queued messages.
			ws->send(ret.dump(), uWS::OpCode::TEXT);
			websocket::get()->addUser(user.username, ws);
			nlohmann::json retAuth;
			retAuth["body"] = {};
			retAuth["event"] = "authed";
			ws->send(retAuth.dump(), uWS::OpCode::TEXT);
			return;
		} else {
			nlohmann::json ret;
			ret["body"] = {{"success", false}, {"message", "Failed to verify details, please try again."}};
			ret["event"] = "login";
			
			ws->send(ret.dump(), uWS::OpCode::TEXT);
			return;
		}
	});

	ws->registerEvent("register", [](uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		if(ws->getUserData()->authed) {
			// ignore if already authed.
			return;
		}
		Types::UserInfo newUser(message);

		if(!newUser.isValid()) {
			nlohmann::json ret;
			ret["body"] = {{"success", false}, {"message", "Info provided was invalid."}};
			ret["event"] = "register";
			
			ws->send(ret.dump(), uWS::OpCode::TEXT);
			return;
		}

		// Hash Password

		std::string hashedPassword = utils::hashPassword(newUser.password);

		if(hashedPassword.length() <= 0) {
			// failed to hash password :[
			
			nlohmann::json ret;
			ret["body"] = {{"success", false}, {"message", "Failed to hash password."}};
			ret["event"] = "register";
			
			ws->send(ret.dump(), uWS::OpCode::TEXT);
			return;
		}

		newUser.password = hashedPassword;

		if(db::get()->registerUser(newUser)) {
			nlohmann::json ret;
			ret["body"] = {{"success", true}, {"message", ""}};
			ret["event"] = "register";
			
			ws->getUserData()->authed = true;
			ws->getUserData()->username = newUser.username;
			websocket::get()->addUser(newUser.username, ws);
			ws->send(ret.dump(), uWS::OpCode::TEXT);
			nlohmann::json retAuth;
			retAuth["body"] = {};
			retAuth["event"] = "authed";
			ws->send(retAuth.dump(), uWS::OpCode::TEXT);
			return;
		} else {
			nlohmann::json ret;
			ret["body"] = {{"success", false}, {"message", "Failed to insert user into Database."}};
			ret["event"] = "register";
			
			ws->send(ret.dump(), uWS::OpCode::TEXT);
			return;
		}
	});

	ws->registerEvent("username_check", [](uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		if(message.is_string()) {
			std::string usernameToCheck = message.get<std::string>();
			
			bool usernameAvailable = db::get()->checkUsername(usernameToCheck);
			nlohmann::json ret;
			ret["body"] = {{"username", usernameToCheck}, {"available", usernameAvailable}};
			ret["event"] = "username_check";
			ws->send(ret.dump(), uWS::OpCode::TEXT);
		}
	});

	ws->getApp().run();
	
	return 0;
}
