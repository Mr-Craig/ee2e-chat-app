     #pragma once
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <openssl/ec.h>
#include "../Utils/Crypto.h"
#include <unordered_map>
#include <cppcodec/base64_url_unpadded.hpp>
#include <cppcodec/base64_rfc4648.hpp>
#include <nlohmann/json.hpp>

#define ONE_DAY 86400
#define SUB	"mailto:1921211@brunel.ac.uk"
#define CURVE NID_X9_62_prime256v1

// https://blog.mozilla.org/services/2016/08/23/sending-vapid-identified-webpush-notifications-via-mozillas-push-service/
// VAPID is just a JWT
struct Vapid
{
	Vapid(std::string& ad, utils::crypto::EcKeyPtr pk) : aud(ad), privKey(pk) {};

	std::string getHeader() 
	{
		nlohmann::json jHead;
		jHead["type"] = "JWT";
		jHead["alg"] = "ES256";
		return jHead.dump();
	}

	std::string getBody()
	{
		nlohmann::json j;
		j["sub"] = SUB;
		j["aud"] = formatAud();

		std::chrono::system_clock::duration now = std::chrono::system_clock::now().time_since_epoch();
		long long seconds = std::chrono::duration_cast<std::chrono::seconds>(now).count();
		j["exp"] = seconds + ONE_DAY;
		return j.dump();
	}

	std::string generateJWT() {

		std::string body = cppcodec::base64_url_unpadded::encode(getBody());
		std::string header = cppcodec::base64_url_unpadded::encode(getHeader());
		std::string baseString = header + "." + body;

		std::vector<uint8_t> strBytes = std::vector<uint8_t>(baseString.begin(), baseString.end());
		std::vector<uint8_t> buffer = utils::crypto::HashSHA256(strBytes);

		ECDSA_SIG* signature = ECDSA_do_sign(buffer.data(), buffer.size(), privKey->getRawPtr());

		std::vector<uint8_t> signatureBytes;

		const BIGNUM* r = NULL, * s = NULL;

		ECDSA_SIG_get0(signature, &r, &s);

		signatureBytes.resize(BN_num_bytes(r) + BN_num_bytes(s));

		BN_bn2bin(r, signatureBytes.data());
		BN_bn2bin(s, &signatureBytes.data()[BN_num_bytes(r)]);

		std::string signatureBase64URL = cppcodec::base64_url_unpadded::encode(signatureBytes);
		return std::string(header + "." + body + "." + signatureBase64URL);
	}

	std::string formatAud() {
		std::string ret = "";

		int tmp = 0;
		for (int i = 0; i < aud.size(); i++) {
			char audChar = aud[i];

			if (audChar == '/') {
				tmp++;
			}
			if (tmp == 3) {
				ret = aud.substr(0, i);
				break;
			}
		}
		return ret;
	}

	utils::crypto::EcKeyPtr privKey;
	std::string aud;
};