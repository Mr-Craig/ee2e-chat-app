#pragma once

#include <memory>
#include <uwebsockets/App.h>	
#include <nlohmann/json.hpp>

class websocket
{
public:
	struct userData {
		std::string id;
		std::string username;
		std::string session_id;
		bool authed;
	};

	static std::shared_ptr<websocket> get();

	void registerEvent(std::string_view event, std::function<void(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message)> func);
private:
	static std::shared_ptr<websocket> socket_ptr;

	std::thread wsThread;

	void onOpen(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws);
	void onMessage(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, std::string_view message, uWS::OpCode opCode);
	void onClose(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, int code, std::string_view message);

	uWS::SSLApp app = uWS::SSLApp({
		.key_file_name = "key.pem",
		.cert_file_name = "cert.pem",
		.passphrase = "1234"
		});

	std::unordered_map<std::string /*id*/, uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* /*socket*/> connectedClients;
	std::unordered_map < std::string_view /*event*/, std::vector< std::function<void(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message)>> /*functions*/> events;
public:
	uWS::SSLApp& getApp() { return app; };

	void init();
};