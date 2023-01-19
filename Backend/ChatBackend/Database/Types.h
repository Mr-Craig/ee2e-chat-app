#pragma once

#include <nlohmann/json.hpp>

using nlohmann::json;

namespace Types
{
    struct UserInfo
    {
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
}
