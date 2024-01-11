#include <string>
#include <set>
#include <openssl/evp.h>
#include "../Utils/Crypto.h"
#include "../Utils/WebAuthN.h"
#include "../Utils/Misc.h"
#include <unordered_map>

namespace Handlers
{
	static std::set<std::string> challenges;

	static void handleBeginWebAuthNRegister(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		std::string username = message["username"].get<std::string>();

		if (!db::get()->checkUsername(username)) {
			ws->send(Types::BasicResponse("register", "Username is taken.", false).toJson(), uWS::OpCode::TEXT);
			return;
		}

		std::tuple<std::string /*challenge*/, std::string /*return to client*/> publicKeyCreationOptions = utils::webauthn::generatePublicKeyCredentialOptions(username);
		nlohmann::json j;

		j["event"] = "begin_passwordless_register";
		j["body"] = std::get<1>(publicKeyCreationOptions);

		// need to use this later to prevent replay attacks (https://w3c.github.io/webauthn/#sctn-cryptographic-challenges)
		challenges.insert(std::get<0>(publicKeyCreationOptions));
		ws->send(j.dump(), uWS::OpCode::TEXT);
	}

	static void handleFinishWebAuthNRegister(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		//https://webauthn.guide/#registration
		std::vector<uint8_t> clientDataUtf8 = message["clientDataJSON"].get<std::vector<uint8_t>>();
		std::vector<uint8_t> attestationObjectUtf8 = message["attestationObject"].get<std::vector<uint8_t>>();
		std::string clientDataDecoded(clientDataUtf8.begin(), clientDataUtf8.end());

		nlohmann::json clientDataJSON = nlohmann::json::parse(clientDataDecoded);

		// check challenge is valid
		std::string challengeBase64URL = clientDataJSON["challenge"].get<std::string>();
		std::vector<uint8_t> challengeBytes = base64::decode(challengeBase64URL);
		std::string challengeHexString = hex::encode(challengeBytes);

		if (challenges.find(challengeHexString) == challenges.end()) {
			// couldn't find challenge
			ws->send(Types::BasicResponse("register", "Challenge doesn't match, or doesn't exist.", false).toJson(), uWS::OpCode::TEXT);
			return;
		}

		// delete challenge so it can't be replayed.
		challenges.erase(challengeHexString);

		nlohmann::json attestationObjectJSON = nlohmann::json::from_cbor(attestationObjectUtf8.begin(), attestationObjectUtf8.end());
		nlohmann::json::binary_t& authDataBytes = attestationObjectJSON["authData"].get_binary();

		std::vector<uint8_t> idLenBytes = utils::misc::sliceVector(authDataBytes, 53, 55);
		//https://stackoverflow.com/questions/59447181/what-is-best-way-to-convert-two-uint8-bytes-to-uint16-byte-in-arduino
		uint16_t credentialIdLength = ((uint16_t)idLenBytes[0] << 8) | idLenBytes[1];

		std::vector<uint8_t> credentialIdBytes = utils::misc::sliceVector(authDataBytes, 55, 55 + credentialIdLength);
		std::string credentialId = std::string(credentialIdBytes.begin(), credentialIdBytes.end());
		std::vector<uint8_t> publicKeyBytes = utils::misc::sliceVector(authDataBytes, 55 + credentialIdLength, authDataBytes.size());

		utils::webauthn::WebAuthNKey keyData(publicKeyBytes);

		if (!keyData.isValid()) {
			WARNING("WebAuthN", "Invalid Key Data");
			ws->send(Types::BasicResponse("register", "Key is invalid", false).toJson(), uWS::OpCode::TEXT);
			return;
		}

		Types::UserInfo newUser;
		newUser.username = message["username"].get<std::string>();
		newUser.password = "";

		utils::crypto::EcKeyPtr EcKey = std::make_unique<utils::crypto::EcKey>(hex::encode(keyData.X), hex::encode(keyData.Y), "");

		if (!EcKey->isValid()) {
			WARNING("WebAuthN", "Invalid Key Data");
			ws->send(Types::BasicResponse("register", "Key is invalid", false).toJson(), uWS::OpCode::TEXT);
			return;
		}

		newUser.publicKey = EcKey->getPublicKeyHex();

		if (db::get()->registerUser(newUser)) {
			ws->getUserData()->authed = true;
			ws->getUserData()->username = newUser.username;
			websocket::get()->addUser(newUser.username, ws);
			ws->send(Types::BasicResponse("register", "", true).toJson(), uWS::OpCode::TEXT);
			ws->send(Types::AuthedMessage(newUser.username, notifs::get()->getPublicKey()).toJson(), uWS::OpCode::TEXT);
		}
		else {
			ws->send(Types::BasicResponse("register", "Unable to insert user into database.", false).toJson(), uWS::OpCode::TEXT);
		}
	}

	static void handleBeginWebAuthNLogin(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		std::string credentialIdBase64URL = message["credentialId"].get<std::string>();
		std::vector<uint8_t> credentialIdBase64Bytes = base64::decode(credentialIdBase64URL);
		std::string credentialId = hex::encode(credentialIdBase64Bytes);

		std::tuple<std::string /*challenge*/, std::string /*json request*/> requestOptions = utils::webauthn::generatePublicKeyRequestOptions(credentialId);

		challenges.insert(std::get<0>(requestOptions));

		nlohmann::json j;
		j["event"] = "begin_login_passwordless";
		j["body"] = std::get<1>(requestOptions);
		ws->send(j.dump(), uWS::OpCode::TEXT);
	}


	static void handleFinishWebAuthNLogin(uWS::WebSocket<(bool)true, (bool)true, websocket::userData>* ws, nlohmann::json message) {
		std::vector<uint8_t> clientDataUtf8 = message["clientDataJSON"].get<std::vector<uint8_t>>();
		std::vector<uint8_t> userHandleUtf8 = message["userHandle"].get<std::vector<uint8_t>>();

		std::string clientDataDecoded(clientDataUtf8.begin(), clientDataUtf8.end());
		std::string userHandle(userHandleUtf8.begin(), userHandleUtf8.end());
		std::string username = message["username"].get<std::string>();

		nlohmann::json clientDataJSON = nlohmann::json::parse(clientDataDecoded);

		if (userHandle != username) {
			ERR("WebAuthN", "Credential username doesn't match username provided.");
			ws->send(Types::BasicResponse("login", "Username mismatch", false).toJson(), uWS::OpCode::TEXT);
			return;
		}

		std::vector<uint8_t> challengeBytes = base64::decode(clientDataJSON["challenge"].get<std::string>());
		std::string challengeHexString = hex::encode(challengeBytes);

		if (challenges.find(challengeHexString) == challenges.end()) {
			ws->send(Types::BasicResponse("login", "Challenge doesn't match, or doesn't exist.", false).toJson(), uWS::OpCode::TEXT);
			return;
		}

		challenges.erase(challengeHexString);

		std::string publicKeyHex = db::get()->getPublicKey(userHandle);

		utils::crypto::EcKeyPtr EcKey = std::make_unique<utils::crypto::EcKey>(publicKeyHex, "");

		if (!EcKey->isValid()) {
			ws->send(Types::BasicResponse("login", "Failed Key Verification", false).toJson(), uWS::OpCode::TEXT);
			WARNING("WebAuthN", "Key is invalid");
			return;
		}

		std::vector<uint8_t> hashedClientDataBytes = utils::crypto::HashSHA256(clientDataUtf8);

		std::string authenticatorData = hex::encode(message["authenticatorData"].get<std::vector<uint8_t>>());
		std::string hashedClientData = hex::encode(hashedClientDataBytes);

		std::string signedData = authenticatorData + hashedClientData;
		std::vector<uint8_t> signedDataBytes = hex::decode(signedData);

		std::vector<uint8_t> digest = utils::crypto::HashSHA256(signedDataBytes);

		std::vector<uint8_t> signatureBytes = message["signature"].get<std::vector<uint8_t>>();
		const uint8_t* signatureBytesPtr = signatureBytes.data();

		int verify_status = ECDSA_verify(0, digest.data(), SHA256_DIGEST_LENGTH, signatureBytes.data(), signatureBytes.size(), EcKey->getRawPtr());

		if (verify_status == 1) {
			ws->getUserData()->authed = true;
			ws->getUserData()->username = userHandle;
			ws->send(Types::BasicResponse("login", "", true).toJson(), uWS::OpCode::TEXT);
			ws->send(Types::AuthedMessage(userHandle, notifs::get()->getPublicKey()).toJson(), uWS::OpCode::TEXT);
			websocket::get()->addUser(userHandle, ws);
		} else {
			ws->send(Types::BasicResponse("login", "Unable to verify signature, please try again.", false).toJson(), uWS::OpCode::TEXT);
		}
	}
}