#pragma once
#include <string>
#include <unordered_map>
#include "../Socket/Socket.h"
#include "WebAuthN.h"
#include "Login.h"
#include "Register.h"
#include "UsernameCheck.h"
#include "Notifications.h"
#include "Relay.h"

#define REGISTER_CALLBACK(evt, func) { \
	websocket::get()->registerEvent(evt, func); \
}

namespace Handlers
{
	static void registerHandlers() {
		/* WebAuthN Handlers */
		REGISTER_CALLBACK("begin_passwordless_register", Handlers::handleBeginWebAuthNRegister);
		REGISTER_CALLBACK("finish_passwordless_register", Handlers::handleFinishWebAuthNRegister);
		REGISTER_CALLBACK("begin_login_passwordless", Handlers::handleBeginWebAuthNLogin);
		REGISTER_CALLBACK("finish_login_passwordless", Handlers::handleFinishWebAuthNLogin);

		/* Regular Auth Handlers */
		REGISTER_CALLBACK("login", Handlers::handleLogin);
		REGISTER_CALLBACK("register", Handlers::handleRegister);

		/* Misc */
		REGISTER_CALLBACK("relay", Handlers::handleRelayEvent);
		REGISTER_CALLBACK("username_check", Handlers::handleCheckUsername);
		REGISTER_CALLBACK("notifications", Handlers::handleNotificationEvent);
	}
}