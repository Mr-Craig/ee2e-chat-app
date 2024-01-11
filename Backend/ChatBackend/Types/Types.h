#pragma once

#include <nlohmann/json.hpp>
#include <vector>

using nlohmann::json;

namespace Types
{
    struct UserInfo
    {
        UserInfo() {};
        UserInfo(json& j) {
            if(j.contains("username") && j["username"].is_string()) {
                username = j["username"].get<std::string>();
            }

            if(j.contains("password") && j["password"].is_string()) {
                password = j["password"].get<std::string>();
            }
        }
    
        bool isValid() { return username.length() >= 4 && password.length() >= 6; };

        std::string username;
        std::string password;
        std::string publicKey;
    };

    struct QueuedMessage
    {
        QueuedMessage(std::string w, std::string m) : who(w), message(m)
        {
        }
        const std::string who;
        const std::string message;
    };

    struct BasicResponse
    {
        BasicResponse(std::string evnt, std::string msg, bool bSuccess) : message(msg), success(bSuccess), event(evnt)
        {
        }

        std::string toJson() {
            nlohmann::json j;
		    j["body"] = {{"success", success}, {"message", message}};
		    j["event"] = event;

		    return j.dump();
        }
	    std::string event;
        std::string message;
        bool success;
    };

    struct AuthedMessage
    {
        AuthedMessage(std::string usrname, std::string nk) : username(usrname), notifKey(nk)
        {
        }

        std::string toJson() {
            nlohmann::json j;
            j["body"] = { {"username", username}, {"key", notifKey}};
            j["event"] = "authed";

            return j.dump();
        }

        std::string notifKey;
        std::string username;
    };

    struct NotificationSub
    {
        NotificationSub() {};
        NotificationSub(std::string& jsonString) {
            nlohmann::json j = nlohmann::json::parse(jsonString);

            endpoint = j["endpoint"].get<std::string>();
            expirationTime = j["expirationTime"].is_number() ? j["expirationTime"].get<int>() : NULL;
            keys.auth = j["keys"]["auth"].get<std::string>();
            keys.p256dh = j["keys"]["p256dh"].get<std::string>();
        };

        std::string endpoint;
        long expirationTime;

        struct NotificationKeys
        {
            NotificationKeys() {};
            std::string auth;
            std::string p256dh;
        };

        NotificationKeys keys;
    };
}
