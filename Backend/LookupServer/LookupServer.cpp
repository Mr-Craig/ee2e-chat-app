#include "LookupServer.h"
#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>
#include <fmt/core.h>

using nlohmann::json;

namespace info
{
    struct serverInformation {
        std::string server_ip;
        int last_ping;
        
    };
}

std::unordered_map<std::string /* server_name */, info::serverInformation> allServers;

// one day
#define MAXIMUM_TIME_BEFORE_INVALID 86400

int main() {
	uWS::SSLApp app = uWS::SSLApp({
		.key_file_name = "key.pem",
		.cert_file_name = "cert.pem",
		.passphrase = "123456789"
	});

    // read current server list
    std::ifstream file("servers.json");

    if(file.good()) {
        json j = json::parse(file);

        if(j.is_array()) {
            for(auto jsonValue : j) {
                std::string serverIp = jsonValue["serverIp"].get<std::string>();
                int lastPing = jsonValue["lastPing"].get<int>();

                info::serverInformation& inf = allServers[serverIp];
                inf.server_ip = serverIp;
                inf.last_ping = lastPing;
            }
        }
    }

    // send server list on all routes because why not
    app.get("/*", [](uWS::HttpResponse<true>* res, uWS::HttpRequest* req) {
        // TODO: should be a faster way of doing this.
        int timeNow = std::time(0) - MAXIMUM_TIME_BEFORE_INVALID;
        json jsonOutput = json::array();
        for(auto kv : allServers) {
            if(kv.second.last_ping > timeNow) {
                jsonOutput.push_back(kv.second.server_ip);
            }
        }
        res->end(jsonOutput.dump());
    });

    // servers will ping every hour if opted in
    app.post("/ping", [](uWS::HttpResponse<true>* res, uWS::HttpRequest* req) {
        res->onData([res, bodyBuffer = (std::string*)nullptr](std::string_view chunk, bool isLast) mutable {
            if(!bodyBuffer) {
                bodyBuffer = new std::string;
            }

            bodyBuffer->append(chunk);

            if(isLast) {
                info::serverInformation& inf = allServers[*bodyBuffer];
                inf.server_ip = *bodyBuffer;
                inf.last_ping = std::time(0);

                std::cout << "Server Ping: " << *bodyBuffer << std::endl;

                if(!res->hasResponded()) {
                    res->writeStatus("200 OK");
                    res->end("pong");
                }

                // write all servers to file
                json j = json::array();

                for(auto kv : allServers) {
                    std::string formattedJsonString = fmt::format("\"serverIp\": \"{}\", \"lastPing\": {}", kv.second.server_ip, kv.second.last_ping);
                    j.push_back(json::parse("{"+formattedJsonString+"}"));
                }

                std::string jsonOutput = j.dump(4);

                std::ofstream file("servers.json");
                file << jsonOutput;
                file.close();
            }
        });

        res->onAborted([res]() {
            res->writeStatus("400 Bad Request");
            res->end("Bad Request");
        });
    });

    app.listen(9876, [](auto socket) {
        if(socket) {
            std::cout << "Listening on port 9876" << std::endl;
        }
    });

    app.run();
    return 0;
}