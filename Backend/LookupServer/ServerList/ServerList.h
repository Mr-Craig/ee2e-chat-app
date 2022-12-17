#pragma once
#include "../LookupServer.h"

using nlohmann::json;

#define MAXIMUM_TIME_BEFORE_INVALID 86400
#define SERVERS_FILE_NAME "servers.json"

class serverList
{
public:
    static std::shared_ptr<serverList> get();

    void updateServer(const std::string& serverIp, std::string_view geoLocation);

    const std::string& const getOutput() { return outputString; };
private:
    static std::shared_ptr<serverList> classPtr;

    void loadServersFromFile();
    void saveServersToFile();

    void updateOutput(bool async = false);

    struct serverInformation {
        std::string serverIp;
        time_t lastPing;
        std::string geolocation;

        json toJson() {
            std::string formattedJsonString = fmt::format("\"serverIp\": \"{}\", \"lastPing\": {}, \"geolocation\": \"{}\"", serverIp, lastPing, geolocation);
            return json::parse("{"+formattedJsonString+"}");
        }
    };

    // contains all server information
    std::unordered_map<std::string, serverInformation> allServers;

    // contains current server list formatted as json
    std::string outputString = "[]";
};