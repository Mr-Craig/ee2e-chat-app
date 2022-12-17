#include "ServerList.h"

std::shared_ptr<serverList> serverList::classPtr = nullptr;

std::shared_ptr<serverList> serverList::get()
{
    if(!classPtr) {
        classPtr = std::make_shared<serverList>();
        classPtr->loadServersFromFile();
    }
    
    return classPtr;
}

void serverList::loadServersFromFile()
{
    std::ifstream file(SERVERS_FILE_NAME);

    if(!file.good())
        return;
    
    json j = json::parse(file);

    if(!j.is_array())
        return;
        
    for(auto jsonValue : j) {
        try {
            serverInformation info;
            info.geolocation = jsonValue["geolocation"].get<std::string>();
            info.lastPing = jsonValue["lastPing"].get<int>();
            info.serverIp = jsonValue["serverIp"].get<std::string>();

            allServers[info.serverIp] = info;
        } catch(json::parse_error& e) {
            std::cout << "Failed to parse JSON file, e: " << e.what() << std::endl;
        }
    }

    updateOutput();
}

void serverList::saveServersToFile()
{
    json jArray = json::array();

    for(auto &kv : allServers) {
        jArray.push_back(kv.second.toJson());
    }

    std::string jsonOutput = jArray.dump(4);

    std::ofstream file(SERVERS_FILE_NAME);
    file << jsonOutput;
    file.close();
}

void serverList::updateOutput(bool async)
{
    if(async) {
        // silence warning
        static_cast<void>(std::async(std::launch::async, [this]() { updateOutput(); }));
        return;
    }

    json jsonOutput = json::array();

    time_t timeNow = std::time(0) - MAXIMUM_TIME_BEFORE_INVALID;

    for(auto kv : allServers) {
        if(kv.second.lastPing > timeNow) {
            json jObject = json::object();

            jObject["serverIp"] = kv.second.serverIp;
            jObject["geolocation"] = kv.second.geolocation;

            jsonOutput.push_back(jObject);
        }
    }

    outputString = jsonOutput.dump();
}

void serverList::updateServer(const std::string& serverIp, std::string_view geoLocation)
{
    serverInformation& serverInfo = allServers[serverIp];

    serverInfo.serverIp = serverIp;
    serverInfo.geolocation = geoLocation;
    serverInfo.lastPing = std::time(0);

    updateOutput(true);
    static_cast<void>(std::async(std::launch::async, [this]() { saveServersToFile(); }));
}
