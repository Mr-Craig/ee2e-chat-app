#pragma once

#include <memory>
#include <uwebsockets/App.h>	
#include <nlohmann/json.hpp>

#define DEFAULT_PORT 8443

class websocket
{
public:
	struct userData {
		std::string id = "";
		std::string username = "";
		bool authed = false;
	};

	static std::shared_ptr<websocket> get();

	void registerEvent(std::string_view event, std::function<void(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message)> func);

	int getPort() const { return portNumber; };

	bool isUserOnline(std::string username);
	void addUser(std::string username, uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws);
	void removeUser(std::string username);
	bool relayMessage(std::string username, nlohmann::json message);
private:

	void processQueue(std::string username);
	bool queueMessage(std::string username, nlohmann::json message);
	static std::shared_ptr<websocket> socket_ptr;

	std::thread wsThread;

	int portNumber = DEFAULT_PORT;

	void onOpen(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws);
	void onMessage(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, std::string_view message, uWS::OpCode opCode);
	void onClose(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, int code, std::string_view message);

	uWS::SSLApp app = uWS::SSLApp({
		.key_file_name = "key.pem",
		.cert_file_name = "cert.pem",
		.passphrase = "123456789"
		});

	//td::unordered_map<std::string /*id*/, uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* /*socket*/> connectedClients;
	std::unordered_map<std::string /*username*/, uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* /*socket*/> authedUsers;
	std::unordered_map < std::string_view /*event*/, std::vector< std::function<void(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message)>> /*functions*/> events;
public:
	uWS::SSLApp& getApp() { return app; };

	void init();
};