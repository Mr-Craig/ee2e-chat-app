#include "../Debug/Debug.h"
#include "../Utils/Utils.h"
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
        res->end(":)");
    });
    
    int port = 8443;

    if (const char* port_env = std::getenv("CHATAPP_PORT"))
        port = std::atoi(port_env);

    app.listen(port, [port](auto* socket) {
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

    connectedClients.insert(std::make_pair(socketId, ws));

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
        ERR("Socket", "Unable to parse message, Message: {}", message);
    }
}

void websocket::onClose(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, int code, std::string_view message)
{
    connectedClients.erase(ws->getUserData()->id);
}
