#pragma once

#include <nlohmann/json.hpp>

using nlohmann::json;

struct UserInfo
{
    UserInfo(json& j) {
        
    }
    
    std::string username;
    std::string password;
    std::string publicKey;
}