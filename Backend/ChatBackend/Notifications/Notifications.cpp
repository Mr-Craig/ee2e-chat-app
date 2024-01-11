#include "../Debug/Debug.h"
#include "../Types/Types.h"
#include <curl/curl.h>
#include "../Utils/Http.h"
#include "../Utils/Crypto.h"
#include <ece.h>
#include <openssl/rand.h>
#include <cppcodec/base64_default_url_unpadded.hpp>
#include "../Database/Database.h"
#include "Vapid.h"
#include "Notifications.h"

std::shared_ptr<notifs> notifs::obj = nullptr;

std::shared_ptr<notifs> notifs::get()
{
	if (!obj) {
		obj = std::make_shared<notifs>();
		obj->init();
	}
	return obj;
}

void notifs::init()
{
	if (!readKey())
		EcKey = std::make_shared<utils::crypto::EcKey>();
}

bool notifs::readKey() 
{
	std::ifstream file(KEY_FILE_NAME);

	if (!file.is_open())
		return false;

	std::stringstream fileStream;
	fileStream << file.rdbuf();

	std::string fileContents = fileStream.str();

	EcKey = std::make_shared<utils::crypto::EcKey>(fileContents.substr(0, fileContents.find('.')), fileContents.substr(fileContents.find('.') + 1, fileContents.length()));

	VERBOSE("Notifs", "Read EC Key, Pub: {} Priv: {}", EcKey->getPublicKeyHex(), EcKey->getPrivateKeyHex());

	return EcKey->isValid();
}


void notifs::saveToFile()
{
	std::stringstream ss;
	ss << EcKey->getPublicKeyHex() << "." << EcKey->getPrivateKeyHex();

	std::ofstream out(KEY_FILE_NAME);
	out << ss.str();
	out.close();
	VERBOSE("Notifs", "Saved new Key");
}

void notifs::sendNotification(std::string& username, std::string payload)
{
	std::string notifSubJson = db::get()->getNotificationSub(username);

	if (notifSubJson.empty())
		return;

	Types::NotificationSub Sub(notifSubJson);

	if (Sub.endpoint.length() <= 0)
		return;

	Vapid vapidKey(Sub.endpoint, EcKey);

	std::string jwt = vapidKey.generateJWT();

	size_t paddingLength = 0;

	// convert keys to byte arrays
	std::vector<uint8_t> rawPublicKey = cppcodec::base64_url_unpadded::decode(Sub.keys.p256dh);
	std::vector<uint8_t> authSecret = cppcodec::base64_url_unpadded::decode(Sub.keys.auth);
	std::vector<uint8_t> cipherText(ece_aesgcm_ciphertext_max_length(ECE_WEBPUSH_DEFAULT_RS, paddingLength, payload.size()));

	// these will be populated by the ecec library
	std::vector<uint8_t> salt(ECE_SALT_LENGTH);
	std::vector<uint8_t> rawSenderPublicKey(ECE_WEBPUSH_PUBLIC_KEY_LENGTH);

	size_t cipherTextLength = cipherText.size();

	int err = ece_webpush_aesgcm_encrypt(rawPublicKey.data(), rawPublicKey.size(), authSecret.data(), authSecret.size(), ECE_WEBPUSH_DEFAULT_RS, paddingLength, (uint8_t*)payload.data(), payload.size(), salt.data(), salt.size(), rawSenderPublicKey.data(), rawSenderPublicKey.size(), cipherText.data(), &cipherTextLength);

	if (err != ECE_OK)
		return;

	utils::http::HeaderMap headers;
	headers.insert({ "Encryption", fmt::format("salt={}", cppcodec::base64_url_unpadded::encode(salt)) });
	headers.insert({ "Crypto-Key", fmt::format("dh={}; p256ecdsa={}", cppcodec::base64_url_unpadded::encode(rawSenderPublicKey), getPublicKey()) });
	headers.insert({ "Content-Length", fmt::format("{}", cipherText.size()) });
	headers.insert({ "Content-Type", "application/octet-stream" });
	headers.insert({ "Content-Encoding", "aesgcm" });
	headers.insert({ "Authorization", fmt::format("vapid t={},k={}", jwt, getPublicKey()) });
	headers.insert({ "TTL", "0" });
	headers.insert({ "Urgency", "high" });

	long httpCode = utils::http::POST(Sub.endpoint, cipherText, headers);

	if (httpCode != 201) {
		WARNING("Notif", "Error sending WebPush to {}\n Got Http Code: {}", Sub.endpoint, httpCode);
	}
}

void notifs::processRelayNotification(std::string username, nlohmann::json& event)
{
	std::string eventName = event["event"].get<std::string>();

	if (eventName == "chatRequest")
	{
		std::string who = event["body"]["username"].get<std::string>();
		int contactState = event["body"]["contactState"].get<int>();

		switch (contactState) {
			case 3 /*SENT*/:
				sendNotification(username, fmt::format("You've got a new request from {}", who));
				break;
			case 0 /*ACCEPTED*/:
				sendNotification(username, fmt::format("{} has accepted your request", who));
				break;
			default:
				break;
		}
	}
	else if (eventName == "newMessage") {
		std::string who = event["body"]["username"].get<std::string>();
		sendNotification(username, fmt::format("New message from {}", who));
	}
}
