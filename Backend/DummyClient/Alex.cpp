#include "Alex.h"
#include <openssl/ec.h>
#include <cppcodec/base64_rfc4648.hpp>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/params.h>

Alex::Alex(Socket* sockPtr, std::string username, std::string password)
{
	socketPtr = sockPtr;

	this->username = username;
	this->password = password;
	socketPtr->setMessageHandler(std::bind(&Alex::handleMessage, this, std::placeholders::_1));
	socketPtr->setOpenHandler(std::bind(&Alex::handleOpen, this));
}

void Alex::handleOpen()
{
	loadSecrets();

	nlohmann::json j;
	j["event"] = "login";
	j["body"] = nlohmann::json::object();
	j["body"]["username"] = username;
	j["body"]["password"] = password;

	socketPtr->sendMessage(j);
}

void Alex::handleMessage(nlohmann::json j)
{
	std::string event = j["event"];

	if (event == "chatRequest") {
		handleContactRequest(j["body"]);
	}
	else if (event == "keyExchange") {
		handleKeyExchange(j["body"]);
	}
	else if (event == "newMessage") {
		handleChatMessage(j["body"]);
	}
	else if (event == "authed") {
		username = j["body"]["username"];
		LOG("Socket", "Authed! Username: {}", username);
	}
	else if (event == "login") {
		if (!j["body"]["success"].get<bool>()) {
			FATAL("Alex", "Failed to login, account must already be created before launching.");
		}
	}
}

void Alex::handleContactRequest(nlohmann::json body)
{
	EContactState state = body["contactState"].get<EContactState>();

	if (state == EContactState::SENT) {
		nlohmann::json reply;

		reply["event"] = "chatRequest";
		reply["body"] = nlohmann::json::object();
		reply["body"]["username"] = username;
		reply["body"]["contactState"] = EContactState::ACCEPTED;

		nlohmann::json msg = constructRelayMessage(body["username"].get<std::string>(), reply);
		socketPtr->sendMessage(msg);
	}
}

void Alex::handleKeyExchange(nlohmann::json body)
{
	EC_KEY* key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
	const EC_GROUP* group = EC_KEY_get0_group(key);

	if (!key) {
		ERR("Alex", "Failed to create keycurve, Error: {}", ERR_get_error());
		return;
	}

	if (!EC_KEY_generate_key(key)) {
		ERR("Alex", "Failed to generate key, Error: {}", ERR_get_error());
		return;
	}

	std::vector<uint8_t> spkiPublicKey = cppcodec::base64_rfc4648::decode(body["publicKey"].get<std::string>());
	const uint8_t* pKeyPtr = spkiPublicKey.data();
	EC_KEY* otherKey = d2i_EC_PUBKEY(NULL, &pKeyPtr, spkiPublicKey.size());

	if (!otherKey) {
		ERR("Alex", "Failed to decode SPKI, Error: {}", ERR_get_error());
		return;
	}

	EC_POINT const* otherPublicKey = EC_KEY_get0_public_key(otherKey);

	//https://wiki.openssl.org/index.php/Elliptic_Curve_Diffie_Hellman#Using_the_Low_Level_APIs
	std::vector<uint8_t> sharedSecret((EC_GROUP_get_degree(EC_KEY_get0_group(key)) + 7) / 8);

	size_t secretLength = ECDH_compute_key(sharedSecret.data(), sharedSecret.size(), otherPublicKey, key, NULL);
	sharedSecret.resize(secretLength);

	std::vector<uint8_t> salt(16);
	RAND_bytes(salt.data(), 16);

	// https://wiki.openssl.org/index.php/EVP_Key_Derivation
	EVP_KDF* kdf;
	EVP_KDF_CTX* kctx = NULL;
	OSSL_PARAM params[5], * p = params;

	/* Find and allocate a context for the HKDF algorithm */
	if ((kdf = EVP_KDF_fetch(NULL, "hkdf", NULL)) == NULL) {
		ERR("Alex", "EVP_KDF_fetch, Error: {}", ERR_get_error());
		return;
	}
	kctx = EVP_KDF_CTX_new(kdf);
	EVP_KDF_free(kdf);    /* The kctx keeps a reference so this is safe */
	if (kctx == NULL) {
		ERR("Alex", "EVP_KDF_CTX_new, Error: {}", ERR_get_error());
		return;
	}

	/* Build up the parameters for the derivation */
	*p++ = OSSL_PARAM_construct_utf8_string("digest", (char*)SN_sha256, (size_t)strlen(SN_sha256));
	*p++ = OSSL_PARAM_construct_octet_string("salt", salt.data(), (size_t)salt.size());
	*p++ = OSSL_PARAM_construct_octet_string("key", sharedSecret.data(), (size_t)sharedSecret.size());
	
	// client doesn't use info
	*p++ = OSSL_PARAM_construct_octet_string("info", (char*)"", (size_t)0);
	*p = OSSL_PARAM_construct_end();
	if (EVP_KDF_CTX_set_params(kctx, params) <= 0) {
		ERR("Alex", "EVP_KDF_CTX_set_params, Error: {}", ERR_get_error());
		return;
	}

	std::vector<uint8_t> finalKey(32);
	/* Do the derivation */
	if (EVP_KDF_derive(kctx, finalKey.data(), finalKey.size(), NULL) <= 0) {
		ERR("Alex", "EVP_KDF_derive, Error: {}", ERR_get_error());
		return;
	}

	std::vector<uint8_t> ourKey(i2d_EC_PUBKEY(key, NULL));
	uint8_t* buffer = ourKey.data();

	i2d_EC_PUBKEY(key, &buffer);

	std::string saltBase64 = cppcodec::base64_rfc4648::encode(salt);
	std::string keyBase64 = cppcodec::base64_rfc4648::encode(finalKey);
	std::string ourKeyBase64 = cppcodec::base64_rfc4648::encode(ourKey);

	nlohmann::json reply;

	reply["event"] = "finishKeyExchange";
	reply["body"] = nlohmann::json::object();
	reply["body"]["username"] = username;
	reply["body"]["publicKey"] = ourKeyBase64;
	reply["body"]["salt"] = saltBase64;

	nlohmann::json msg = constructRelayMessage(body["username"].get<std::string>(), reply);
	socketPtr->sendMessage(msg);

	sharedSecrets[body["username"].get<std::string>()] = keyBase64;

	saveSecrets();

	LOG("Alex", "Key Exchange result: {}", keyBase64);
}

void Alex::handleChatMessage(nlohmann::json body)
{
	std::vector<uint8_t> tag = cppcodec::base64_rfc4648::decode(body["eTag"].get<std::string>());
	std::vector<uint8_t> ciphertext = cppcodec::base64_rfc4648::decode(body["encryptedData"].get<std::string>());
	std::vector<uint8_t> nonce = cppcodec::base64_rfc4648::decode(body["iv"].get<std::string>());

	std::vector<uint8_t> sharedSecret = cppcodec::base64_rfc4648::decode(sharedSecrets[body["username"].get<std::string>()]);

	//https://wiki.openssl.org/index.php/EVP_Authenticated_Encryption_and_Decryption#Authenticated_Decryption_using_GCM_mode
	std::string decryptedMessage = decryptMessage(ciphertext, sharedSecret, nonce, tag);

	LOG("Alex", "Decrypted Message: {}", decryptedMessage);

	nlohmann::json readReceipt;

	readReceipt["event"] = "messageState";
	readReceipt["body"] = nlohmann::json::object();
	readReceipt["body"]["messageId"] = body["id"].get<std::string>();
	readReceipt["body"]["username"] = username;
	readReceipt["body"]["state"] = EMessageState::READ;

	nlohmann::json readRelay = constructRelayMessage(body["username"].get<std::string>(), readReceipt);
	socketPtr->sendMessage(readRelay);

	std::vector<uint8_t> newTag;
	std::vector<uint8_t> newCT;
	std::vector<uint8_t> newNonce(12);
	RAND_bytes(newNonce.data(), 12);

	bool res = encryptMessage(fmt::format("test message, i can't actually react to your messages :(", decryptedMessage), sharedSecret, newCT, newTag, newNonce);

	nlohmann::json newMessage;

	newMessage["event"] = "newMessage";
	newMessage["body"] = nlohmann::json::object();
	newMessage["body"]["eTag"] = cppcodec::base64_rfc4648::encode(newTag);
	newMessage["body"]["username"] = username;

	const auto time = std::chrono::system_clock::now();

	newMessage["body"]["time"] = (uint64_t)std::chrono::system_clock::to_time_t(time) * 1000;
	newMessage["body"]["encryptedData"] = cppcodec::base64_rfc4648::encode(newCT);
	newMessage["body"]["iv"] = cppcodec::base64_rfc4648::encode(newNonce);
	// lazy
	newMessage["body"]["id"] = body["id"].get<std::string>() + "-ALEX";

	nlohmann::json newMessageRelay = constructRelayMessage(body["username"].get<std::string>(), newMessage);
	socketPtr->sendMessage(newMessageRelay);
}

void Alex::loadSecrets()
{
	std::ifstream file("secrets.json");

	if (!file.good())
		return;

	try
	{
		nlohmann::json j = nlohmann::json::parse(file);
		sharedSecrets = j.get<std::unordered_map<std::string, std::string>>();
		LOG("Alex", "Loaded {} secrets.", sharedSecrets.size());
	}
	catch (std::exception& e) {
		WARNING("Alex", "Failed to load secrets, Error: {}", e.what());
	}
}

void Alex::saveSecrets()
{
	nlohmann::json j = sharedSecrets;

	std::ofstream file("secrets.json");
	file << j.dump();
	file.close();
}

/* https://wiki.openssl.org/index.php/EVP_Authenticated_Encryption_and_Decryption#Authenticated_Decryption_using_GCM_mode */
std::string Alex::decryptMessage(std::vector<uint8_t>& ciphertext, std::vector<uint8_t>& key, std::vector<uint8_t>& iv, std::vector<uint8_t>& tag)
{
	std::vector<uint8_t> ret(ciphertext.size());
	EVP_CIPHER_CTX* ctx;
	int len;
	int plaintext_len;

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new()))
		return std::string();

	/* Initialise the decryption operation. */
	if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
		return std::string();

	/* Set IV length. Not necessary if this is 12 bytes (96 bits) */
	if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv.size(), NULL))
		return std::string();

	/* Initialise key and IV */
	if (!EVP_DecryptInit_ex(ctx, NULL, NULL, key.data(), iv.data()))
		return std::string();

	/*
	 * Provide the message to be decrypted, and obtain the plaintext output.
	 * EVP_DecryptUpdate can be called multiple times if necessary
	 */
	if (!EVP_DecryptUpdate(ctx, ret.data(), &len, ciphertext.data(), ciphertext.size()))
		return std::string();
	plaintext_len = len;

	/* Set expected tag value. Works in OpenSSL 1.0.1d and later */
	if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag.data()))
		return std::string();

	/*
	 * Finalise the decryption. A positive return value indicates success,
	 * anything else is a failure - the plaintext is not trustworthy.
	 */
	int res = EVP_DecryptFinal_ex(ctx, ret.data() + len, &len);

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	if (res > 0) {
		/* Success */
		plaintext_len += len;
		ret.resize(plaintext_len);

		std::string plaintext(ret.begin(), ret.end());
		return plaintext;
	}
	else {
		/* Verify failed */
		return std::string();
	}
}

/* https://wiki.openssl.org/index.php/EVP_Authenticated_Encryption_and_Decryption#Authenticated_Encryption_using_GCM_mode */
bool Alex::encryptMessage(std::string plaintext, std::vector<uint8_t>& key, std::vector<uint8_t>& ciphertext, std::vector<uint8_t>& tag, std::vector<uint8_t>& iv)
{
	//https://stackoverflow.com/a/58888467
	ciphertext.resize(((plaintext.size() + 16) / 16) * 16);

	EVP_CIPHER_CTX* ctx;
	int len;
	int ciphertext_len;


	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new()))
		return false;

	/* Initialise the encryption operation. */
	if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
		return false;

	/*
	 * Set IV length if default 12 bytes (96 bits) is not appropriate
	 */
	if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv.size(), NULL))
		return false;

	/* Initialise key and IV */
	if (1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key.data(), iv.data()))
		return false;

	/*
	 * Provide the message to be encrypted, and obtain the encrypted output.
	 * EVP_EncryptUpdate can be called multiple times if necessary
	 */
	if (1 != EVP_EncryptUpdate(ctx, ciphertext.data(), &len, (unsigned char*)plaintext.data(), plaintext.size()))
		return false;
	ciphertext_len = len;

	/*
	 * Finalise the encryption. Normally ciphertext bytes may be written at
	 * this stage, but this does not occur in GCM mode
	 */
	if (1 != EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len))
		return false;
	ciphertext_len += len;

	tag.resize(16);
	/* Get the tag */
	if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag.data()))
		return false;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	ciphertext.resize(ciphertext_len);
	return true;
}

nlohmann::json Alex::constructRelayMessage(std::string to, nlohmann::json& j)
{
	nlohmann::json relay;
	relay["event"] = "relay";
	relay["body"] = nlohmann::json::object();
	relay["body"]["username"] = to;
	relay["body"]["body"] = j;

	return relay;
}
