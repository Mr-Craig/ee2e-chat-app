#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <uwebsockets/App.h>	
#include <nlohmann/json.hpp>

namespace Handlers
{
	enum EContactState
	{
		ACCEPTED,
		PENDING,
		DENIED,
		SENT,
		/* special case where the server rejects the contact request, only due to the username not existing. */
		REJECTED
	};
	/*relay event structure*/
	/*{
            "event": "relay",
            "body" : {
                "username": username, -- whos it going too
                "body": { -- message to send
                    "event": event,
                    "body": body
                }
            }
        }*/

	static bool shouldCancel(nlohmann::json message) {
		if (message["body"]["event"].get<std::string>() == "chatRequest") {
			nlohmann::json contactBody = message["body"]["body"];

			if (contactBody["contactState"].get<uint8_t>() == EContactState::SENT) {
				std::string contactUsername = message["username"].get<std::string>();
				bool usernameExists = !db::get()->checkUsername(contactUsername);

				return !usernameExists;
			}
		}
		return false;
	}
	static void handleRelayEvent(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message)
	{
		if (!ws->getUserData()->authed)
			return;

		std::string username = message["username"].get<std::string>();

		/*special case*/
#pragma region CheckContactUsername
		if(shouldCancel(message)) {
			nlohmann::json rejectContact;
			rejectContact["event"] = "chatRequest";
			rejectContact["body"] = nlohmann::json::object();
			rejectContact["body"]["username"] = message["username"].get<std::string>();
			rejectContact["body"]["contactState"] = static_cast<uint8_t>(EContactState::REJECTED);
			websocket::get()->relayMessage(message["body"]["body"]["username"].get<std::string>(), rejectContact);
			return;
		}

#pragma endregion

		websocket::get()->relayMessage(username, message["body"]);
		notifs::get()->processRelayNotification(username, message["body"]);
	}
}