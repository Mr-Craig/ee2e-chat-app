#pragma once
#include <argon2.h>
#include "../Debug/Debug.h"
#include "Crypto.h"
#include <openssl/rand.h>
#include <string>

#define HASHLEN 32
#define SALTLEN 16

namespace utils
{
	namespace argon2
	{
		static std::string hashPassword(std::string password)
		{
			uint32_t t_cost = 2;
			uint32_t m_cost = (1 << 16);
			uint32_t parallelism = 2;

			std::string hashedPassword;
			hashedPassword.resize(HASHLEN * 4);

			std::vector<uint8_t> salt = utils::crypto::randomBytes(SALTLEN);

			size_t encodedSize = hashedPassword.size();
			int ret = argon2id_hash_encoded(t_cost, m_cost, parallelism, password.data(), password.size(), salt.data(), SALTLEN, HASHLEN, hashedPassword.data(), encodedSize);

			if (ret != ARGON2_OK) {
				ERR("Hashing", "Failed to hash password! Error: {}", argon2_error_message(ret));
				return "";
			};

			hashedPassword.erase(std::find(hashedPassword.begin(), hashedPassword.end(), '\0'), hashedPassword.end());

			return hashedPassword;
		}

		static bool verifyPasswordHash(std::string password, std::string passwordHash)
		{
			return argon2id_verify(passwordHash.c_str(), password.data(), password.length()) == ARGON2_OK;
		}
	}
}