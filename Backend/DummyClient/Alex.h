#pragma once
#include "Socket.h"


/* types from frontend */
enum EContactState
{
	ACCEPTED,
	PENDING,
	DENIED,
	SENT
};

enum EMessageState
{
	READ
};

class Alex
{
public:
	Alex(Socket* sockPtr, std::string username, std::string password);

private:
	void handleOpen();
	void handleMessage(nlohmann::json j);

	void handleContactRequest(nlohmann::json body);
	void handleKeyExchange(nlohmann::json body);
	void handleChatMessage(nlohmann::json body);

	void loadSecrets();
	void saveSecrets();

	std::string decryptMessage(std::vector<uint8_t>& ciphertext, std::vector<uint8_t>& key, std::vector<uint8_t>& iv, std::vector<uint8_t>& tag);

	bool encryptMessage(std::string plaintext, std::vector<uint8_t>& key, std::vector<uint8_t>& ciphertext, std::vector<uint8_t>& tag, std::vector<uint8_t>& iv);

	nlohmann::json constructRelayMessage(std::string to, nlohmann::json& j);

	std::unordered_map<std::string, std::string> sharedSecrets;

	std::string username;
	std::string password;
	Socket* socketPtr;
};