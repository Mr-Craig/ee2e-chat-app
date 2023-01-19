#include "../Debug/Debug.h"
#include "../Utils/Utils.h"
#include "../Database/Database.h"
#include "Socket.h"

std::shared_ptr<websocket> websocket::socket_ptr = nullptr;

std::shared_ptr<websocket> websocket::get()
{
	if (!socket_ptr) {
		socket_ptr = std::make_shared<websocket>();
		socket_ptr->init();
	}
	return std::shared_ptr<websocket>(socket_ptr);
}

void websocket::registerEvent(std::string_view event, std::function<void(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message)> func)
{
    auto& vectorRef = events[event];
    vectorRef.push_back(func);
}

bool websocket::isUserOnline(std::string username)
{
    return authedUsers.find(username) != authedUsers.end();
}

void websocket::addUser(std::string username, uWS::WebSocket<(bool)true, (bool)true, websocket::userData> *ws)
{
    authedUsers[username] = ws;
    processQueue(username);
}

void websocket::removeUser(std::string username)
{
    authedUsers.erase(username);
}

bool websocket::relayMessage(std::string username, nlohmann::json message)
{
    if(!isUserOnline(username)) {
        if(queueMessage(username, message)) {
            VERBOSE("Queue", "Queued message to be sent to {}", username);
        }
        return false;
    }
    authedUsers[username]->send(message.dump(), uWS::OpCode::TEXT);
    return true;
}

void websocket::processQueue(std::string username)
{
    std::vector<Types::QueuedMessage> QueuedMessage = db::get()->getQueuedMessages(username);
    auto& wsRef = authedUsers[username];
    for(auto& Message : QueuedMessage) 
    {
        wsRef->send(Message.message, uWS::OpCode::TEXT);
        VERBOSE("Queue", "Sent queued message to {}", username);
    }
    db::get()->deleteQueuedMessages(username);
}

bool websocket::queueMessage(std::string username, nlohmann::json message)
{
    Types::QueuedMessage qMessage(username, message.dump());
    return db::get()->queueMessage(qMessage);
}

void websocket::init()
{
	if (app.constructorFailed())
		FATAL("Socket", "App Construction failed.", "");

    app.ws<websocket::userData>("/*", {
        .compression = uWS::CompressOptions::DEDICATED_COMPRESSOR,
        .maxPayloadLength = 16 * 1024 * 1024,
        .idleTimeout = 16,
        .maxBackpressure = 1 * 1024 * 1024,
        .closeOnBackpressureLimit = false,
        .resetIdleTimeoutOnSend = false,
        .sendPingsAutomatically = true,
        .upgrade = nullptr,
        .open = std::bind(&websocket::onOpen, this, std::placeholders::_1),
        .message = std::bind(&websocket::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
        .drain = nullptr,
        .ping = nullptr,
        .pong = nullptr,
        .close = std::bind(&websocket::onClose, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    });

    app.get("/*", [](uWS::HttpResponse<true>* res, uWS::HttpRequest* req) {
        res->writeStatus("404 Not Found");
        res->end("Not Found");
    });

    app.get("/ping", [](uWS::HttpResponse<true>* res, uWS::HttpRequest* req) {
        res->writeHeader("Access-Control-Allow-Origin", "*");
        res->writeHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res->writeHeader("Access-Control-Allow-Headers", "origin, content-type, accept, x-requested-with");
        res->writeHeader("Access-Control-Max-Age", "3600");
        res->writeStatus("200 OK");
        res->end("pong");
    });
    
    unsigned int port = 0;

#ifndef _WIN32
    if (const char* port_env = std::getenv("CHATAPP_PORT"))
        port = std::stoul(port_env);
#else
    // use windows "safe" function
    char* buffer = nullptr;
    size_t bufferSz = 0;
    if(_dupenv_s(&buffer, &bufferSz, "CHATAPP_PORT") == 0 && buffer != nullptr)
        port = std::stoul(buffer);
    free(buffer);
#endif

    if(port != 0) {
        portNumber = port;
    }

    app.listen(portNumber, [port](auto* socket) {
        if (socket) {
            LOG("Socket", "Listening on Port {}", port);
        }
        else {
            FATAL("Socket", "Unable to listen on port {}", port);
        }
    });
}

void websocket::onOpen(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws)
{
    std::string socketId = utils::generateUniqueId("sk");

    ws->getUserData()->id = socketId;

    //connectedClients.insert(std::make_pair(socketId, ws));

    LOG("Socket", "New Connection, Id: {}", socketId);
}

void websocket::onMessage(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, std::string_view message, uWS::OpCode opCode)
{
    using namespace nlohmann;
    try {
        json messageJson = json::parse(message);

        if (messageJson.contains("event")) {
            for (auto& func : events[messageJson["event"].get<std::string>()]) {
                func(ws, messageJson["body"]);
            }
        }
        else {
            WARNING("Socket", "Invalid Message, No Event Specified.", "");
        }
    }
    catch(json::parse_error& e) {
        ERR("Socket", "Unable to parse message, Error: {}", e.what());
    }
}

void websocket::onClose(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, int code, std::string_view message)
{
    //connectedClients.erase(ws->getUserData()->id);
    if(ws->getUserData()->authed) {
        removeUser(ws->getUserData()->username);
    }
}
