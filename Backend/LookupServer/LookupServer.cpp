#include "LookupServer.h"
#include "ServerList/ServerList.h"

using nlohmann::json;

int main() {
    // init class
    std::shared_ptr<serverList> sL = serverList::get();

	uWS::SSLApp app = uWS::SSLApp({
		.key_file_name = "key.pem",
		.cert_file_name = "cert.pem",
		.passphrase = "123456789"
	}).get("/*", [](uWS::HttpResponse<true>* res, uWS::HttpRequest* req) {
        res->writeHeader("Access-Control-Allow-Origin", "*");
        res->writeHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res->writeHeader("Access-Control-Allow-Headers", "origin, content-type, accept, x-requested-with");
        res->writeHeader("Access-Control-Max-Age", "3600");
        res->writeStatus("200 OK");
        res->end(serverList::get()->getOutput());
    }).post("/ping", [](uWS::HttpResponse<true>* res, uWS::HttpRequest* req) {

        
        res->onData([res, req, bodyBuffer = (std::string*)nullptr](std::string_view chunk, bool isLast) mutable {
            if(!bodyBuffer) {
                bodyBuffer = new std::string;
            }

            bodyBuffer->append(chunk);

            if(isLast) {
                serverList::get()->updateServer(*bodyBuffer, req->getHeader("cf-ipcountry"));

                std::cout << "Server Ping: " << *bodyBuffer << std::endl;

                if(!res->hasResponded()) {
                    res->writeStatus("200 OK");
                    res->end("pong");
                }
            }
        });

        res->onAborted([res]() {
            res->writeStatus("400 Bad Request");
            res->end("Bad Request");
        });

    }).listen(9437, [](auto socket) {
        if(socket) {
            std::cout << "Listening on port 9437" << std::endl;
        }
    }).run();

    return 0;
}