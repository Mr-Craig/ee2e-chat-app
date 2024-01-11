#include "Socket.h"

Socket::Socket(std::string & uri)
{
    client.set_access_channels(websocketpp::log::alevel::fail);
    client.clear_access_channels(websocketpp::log::alevel::frame_payload);
    client.set_error_channels(websocketpp::log::elevel::all);

    client.init_asio();

    socketUri = uri;

    client.set_tls_init_handler(std::bind(&Socket::onTlsInit, this, std::placeholders::_1));
    client.set_message_handler(std::bind(&Socket::onMessage, this, std::placeholders::_1, std::placeholders::_2));
    client.set_open_handler(std::bind(&Socket::onOpen, this, std::placeholders::_1));
    client.set_close_handler(std::bind(&Socket::onClose, this, std::placeholders::_1));
    client.set_fail_handler(std::bind(&Socket::onFail, this, std::placeholders::_1));
}

void Socket::sendMessage(nlohmann::json& j)
{
    client_::connection_ptr con = client.get_con_from_hdl(currentHandle);

    if (con) {
        std::string json = j.dump();
        con->send(json.data(), json.size(), websocketpp::frame::opcode::TEXT);
        LOG("Client", "Sent: {}", json);
    }
    else {
        ERR("Client", "Failed to send message, Connection is invalid.");
    }
}

void Socket::onMessage(websocketpp::connection_hdl handle, client_::message_ptr msg)
{
    LOG("Client", "Got Message: {}", msg->get_payload());

    try {
        nlohmann::json j = nlohmann::json::parse(msg->get_payload());

        if (messageHandler) messageHandler(j);
    }
    catch (std::exception& e) {
        WARNING("Client", "Caught Exception: {}", e.what());
    }
}

void Socket::onFail(websocketpp::connection_hdl handle)
{
    websocketpp::client<websocketpp::config::asio_tls_client>::connection_ptr con = client.get_con_from_hdl(handle);
    FATAL("Client", "Failure on Socket, Error Code: {}", con->get_ec().message());
}

void Socket::onOpen(websocketpp::connection_hdl handle)
{
    currentHandle = handle;
    LOG("Client", "Socket Connection Open");
    if (openHandler) openHandler();
}

websocketpp::lib::shared_ptr<boost::asio::ssl::context> Socket::onTlsInit(websocketpp::connection_hdl handle)
{
    websocketpp::lib::shared_ptr<boost::asio::ssl::context> ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12);

    try {
        ctx->set_options(boost::asio::ssl::context::default_workarounds |
            boost::asio::ssl::context::no_sslv2 |
            boost::asio::ssl::context::no_sslv3 |
            boost::asio::ssl::context::single_dh_use);
        
        // don't verify cert since its most likely a self signed one
        ctx->set_verify_mode(boost::asio::ssl::verify_none);
    } catch (std::exception& e) {
        FATAL("Client", "Failed to initailize TLS, error: {}", e.what());
    }

    return ctx;
}

void Socket::onClose(websocketpp::connection_hdl handle)
{
    ERR("Client", "Socket Closed!");
}
