#pragma once
#include <string>
#include <vector>
#include "../Debug/Debug.h"
#include "Misc.h"
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/params.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/core_names.h>
#include <openssl/err.h>
#include <openssl/kdf.h>
#include <openssl/ec.h>

namespace utils
{
	namespace crypto
	{
		constexpr int CURVE_NAME = NID_X9_62_prime256v1;

        static void dumpOpenSSLErrors()
        {
            unsigned long long error = ERR_get_error();
            do
            {
                std::string errorString;
                errorString.resize(128);
                ERR_error_string_n(error, errorString.data(), errorString.size());
                ERR("OpenSSL", "Error: {}", errorString);
                error = ERR_get_error();
            } while (error != 0);
        }

		static std::vector<uint8_t> HashSHA256(std::vector<uint8_t>& data)
		{
			std::vector<uint8_t> buffer;
			buffer.resize(SHA256_DIGEST_LENGTH);

			SHA256_CTX sha256;
			SHA256_Init(&sha256);
			SHA256_Update(&sha256, data.data(), data.size());
			SHA256_Final(buffer.data(), &sha256);

			return buffer;
		}

		static std::vector<uint8_t> randomBytes(int len) 
		{
			std::vector<uint8_t> randomBytes;
			randomBytes.resize(len);
			RAND_bytes(randomBytes.data(), len);
			return randomBytes;
		}

		class EcKey {
		public:
			EcKey() {
				init();
				EC_KEY_generate_key(keyObj);
			}
			EcKey(std::string x, std::string y, std::string d) {
				init();
				BIGNUM* BnX = NULL, * BnY = NULL, * BnD = NULL;

				if (!d.empty()) {
					BN_hex2bn(&BnD, d.c_str());
					EC_KEY_set_private_key(keyObj, BnD);
				}

				BN_hex2bn(&BnX, x.c_str());
				BN_hex2bn(&BnY, y.c_str());

				EC_KEY_set_public_key_affine_coordinates(keyObj, BnX, BnY);

				BN_free(BnX);
				BN_free(BnY);
				BN_free(BnD);
			}
			EcKey(std::string point, std::string priv) {
				init();

				EC_POINT* pubKey = EC_POINT_hex2point(group, point.c_str(), NULL, ctx);
				BIGNUM* privKey = NULL;

				if(!priv.empty())
					BN_hex2bn(&privKey, priv.c_str());

				EC_KEY_set_public_key(keyObj, pubKey);
				EC_KEY_set_private_key(keyObj, privKey);

				EC_POINT_free(pubKey);
				BN_free(privKey);
			}
			~EcKey() {
				EC_KEY_free(keyObj);
				BN_CTX_free(ctx);
			}

			bool isValid() {
				return EC_KEY_check_key(keyObj);
			}

			std::vector<uint8_t> getPublicKeyOct() {
				const EC_POINT* publicKey = EC_KEY_get0_public_key(keyObj);
				std::vector<uint8_t> ret;

				if (publicKey == NULL)
					return ret;

				size_t bufferSize = EC_POINT_point2oct(group, publicKey, POINT_CONVERSION_UNCOMPRESSED, NULL, NULL, ctx);
				ret.resize(bufferSize);
				EC_POINT_point2oct(group, publicKey, POINT_CONVERSION_UNCOMPRESSED, ret.data(), ret.size(), ctx);
				
				return ret;
			}
			std::string getPublicKeyHex() {
				const EC_POINT* publicKey = EC_KEY_get0_public_key(keyObj);
				std::string ret;

				if (publicKey == NULL)
					return ret;

				char* pointHex = EC_POINT_point2hex(group, publicKey, POINT_CONVERSION_UNCOMPRESSED, ctx);
				ret.assign(pointHex, strlen(pointHex));

				return ret;
			}
			std::string getPrivateKeyHex() {
				const BIGNUM* privKey = EC_KEY_get0_private_key(keyObj);
				std::string ret;

				if (privKey == NULL)
					return ret;

				char* privKeyHex = BN_bn2hex(privKey);
				ret.assign(privKeyHex, strlen(privKeyHex));

				return ret;
			}
			std::vector<uint8_t> getPrivateKeyBin() {
				const BIGNUM* privKey = EC_KEY_get0_private_key(keyObj);
				std::vector<uint8_t> ret;

				if (privKey == NULL)
					return ret;

				ret.resize(BN_num_bytes(privKey));
				BN_bn2bin(privKey, ret.data());
				return ret;
			}

			EC_KEY* getRawPtr() { return keyObj; };
		private:
			void init() {
				keyObj = EC_KEY_new_by_curve_name(CURVE_NAME);
				group = EC_KEY_get0_group(keyObj);
				ctx = BN_CTX_new();
			}
			BN_CTX* ctx;
			const EC_GROUP* group;
			EC_KEY* keyObj;
		};

		using EcKeyPtr = std::shared_ptr<EcKey>;
	}
}