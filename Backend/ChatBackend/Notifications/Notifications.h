#pragma once

#include <openssl/err.h>
#include <openssl/ec.h>
#include <vector>
#include <string>
#include <memory>
#include <openssl/pem.h>
#include <nlohmann/json.hpp>
#include "../Utils/Crypto.h"

#define CURVE NID_X9_62_prime256v1
#define KEY_FILE_NAME "key.ec"

class notifs
{
public:
	static std::shared_ptr<notifs> get();
private:
	static std::shared_ptr<notifs> obj;

	void init();

	bool readKey();

	void saveToFile();
public:
	std::string getPublicKey() { return cppcodec::base64_url_unpadded::encode(EcKey->getPublicKeyOct()); };
	void sendNotification(std::string& username, std::string payload);

	void processRelayNotification(std::string username, nlohmann::json& event);
private:
	utils::crypto::EcKeyPtr EcKey;
};