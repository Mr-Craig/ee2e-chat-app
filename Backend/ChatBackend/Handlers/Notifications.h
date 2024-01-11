#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <uwebsockets/App.h>	
#include <nlohmann/json.hpp>

namespace Handlers
{
	static void handleNotificationEvent(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message)
	{
		if (!ws->getUserData()->authed || !message.contains("sub") || !message["sub"].is_object())
			return;

		VERBOSE("Notifs", "New Subscription: {}", ws->getUserData()->username);

		std::string subscriptionData = message["sub"].dump();

		db::get()->updateNotificationSub(ws->getUserData()->username, subscriptionData);

		notifs::get()->sendNotification(ws->getUserData()->username, "You've subscribed to notifications.");
	}
}