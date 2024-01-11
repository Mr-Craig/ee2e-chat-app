#pragma once

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <openssl/rand.h>
#include <cppcodec/hex_default_lower.hpp>
#include <cbor.h>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>
#include <vector>
#include <tuple>

#define CHALLENGE_LEN 16

namespace utils
{
	namespace webauthn
	{
		static std::tuple<std::string /*challenge*/, std::string /*JSON string*/> generatePublicKeyCredentialOptions(std::string username)
		{
			nlohmann::json j;

			std::vector<uint8_t> randomBytes = utils::crypto::randomBytes(CHALLENGE_LEN);

			std::string challenge = hex::encode(randomBytes);

			j["challenge"] = challenge;
			j["rp"] = {
				{"name", "E2EE Chat Backend"},
				{"id", "localhost"}
			};
			j["user"] = {
				{"id", username},
				{"name", username},
				{"displayName", username}
			},
				j["pubKeyCredParams"] = nlohmann::json::array();
			j["pubKeyCredParams"].push_back({
				{"alg", -7},
				{"type", "public-key"}
				});
			j["authenticatorSelection"] = {
				{"authenticatorAttachment", "platform"},
				{"userVerification", "preferred"}
			};
			j["timeout"] = 60000;
			j["attestation"] = "direct";
			return std::tuple<std::string, std::string>(challenge, j.dump());
		}

		static std::tuple<std::string /*challenge*/, std::string /*JSON string*/> generatePublicKeyRequestOptions(std::string credentialId)
		{
			nlohmann::json j;


			std::string challenge = hex::encode(utils::crypto::randomBytes(CHALLENGE_LEN));

			j["challenge"] = challenge;
			j["allowCredentials"] = nlohmann::json::array();
			j["allowCredentials"].push_back({
				{"id", credentialId},
				{"type", "public-key"},
				{"transports", nlohmann::json::array()}
				});
			j["allowCredentials"][0]["transports"].push_back("usb");
			j["allowCredentials"][0]["transports"].push_back("internal");
			j["allowCredentials"][0]["transports"].push_back("ble");
			j["allowCredentials"][0]["transports"].push_back("nfc");
			j["timeout"] = 60000;

			return std::tuple<std::string, std::string>(challenge, j.dump());
		}

		struct WebAuthNKey
		{
			WebAuthNKey() {};
			WebAuthNKey(std::vector<uint8_t>& byteArray) {
				struct cbor_load_result res;
				cbor_item_t* item = cbor_load(byteArray.data(), byteArray.size(), &res);
				size_t mapSize = 0;
				cbor_pair* mapHandle = nullptr;

				if (res.error.code != CBOR_ERR_NONE) goto end;

				mapSize = cbor_map_size(item);

				if (mapSize != 5) goto end;

				mapHandle = cbor_map_handle(item);

				Type = getUInt(mapHandle[0].value);
				Algorithm = getInt(mapHandle[1].value);
				Curve = getUInt(mapHandle[2].value);
				getByteString(mapHandle[3].value, X);
				getByteString(mapHandle[4].value, Y);

				end:
				cbor_decref(&item);
			};

		private:
			uint8_t getUInt(cbor_item_t* item) { return cbor_typeof(item) == CBOR_TYPE_UINT ? cbor_get_uint8(item) : -1; };
			int8_t getInt(cbor_item_t* item) { return cbor_typeof(item) == CBOR_TYPE_NEGINT ? -cbor_get_uint8(item) - 1 : -1; };
			void getByteString(cbor_item_t* item, std::vector<uint8_t>& vec) { 
				if (cbor_typeof(item) != CBOR_TYPE_BYTESTRING) return;
				uint8_t* byteStringPtr = cbor_bytestring_handle(item);
				std::copy(&byteStringPtr[0], &byteStringPtr[cbor_bytestring_length(item)], std::back_inserter(vec));
			}
		public:

			bool isValid() { return Type == 2 && Algorithm == -7 && Curve == 1 && X.size() == 32 && Y.size() == 32; };
			uint8_t Type;
			int8_t Algorithm;
			uint8_t Curve;
			std::vector<uint8_t> X;
			std::vector<uint8_t> Y;
		};
	}
}