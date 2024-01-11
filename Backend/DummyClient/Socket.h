#pragma once
#include <string>
#include <iostream>
#include "../ChatBackend/Debug/Debug.h"
#include <websocketpp/client.hpp>
#include <nlohmann/json.hpp>
#include <websocketpp/config/asio_client.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client_;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

class Socket
{
public:
    Socket(std::string& uri);
    void run() { 
        websocketpp::lib::error_code ec;
        client_::connection_ptr con = client.get_connection(socketUri, ec);

        client.connect(con);

        client.run(); 
    };

    void setMessageHandler(std::function<void(nlohmann::json)> func) { messageHandler = func; };
    void setOpenHandler(std::function<void()> func) { openHandler = func; };

    void sendMessage(nlohmann::json& j);
private:
    client_ client;
    std::string socketUri;

    void onMessage(websocketpp::connection_hdl handle, client_::message_ptr msg);
    void onFail(websocketpp::connection_hdl handle);

    void onOpen(websocketpp::connection_hdl handle);
    websocketpp::lib::shared_ptr<boost::asio::ssl::context> onTlsInit(websocketpp::connection_hdl handle);
    void onClose(websocketpp::connection_hdl handle);

    websocketpp::connection_hdl currentHandle;

    std::function<void(nlohmann::json)> messageHandler;
    std::function<void()> openHandler;
};
