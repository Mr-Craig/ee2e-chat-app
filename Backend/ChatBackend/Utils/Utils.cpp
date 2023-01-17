#include <future>
#include <chrono>
#include <thread>
#include <argon2.h>
#include <openssl/rand.h>

#include "../Debug/Debug.h"
#include "Utils.h"

std::string utils::generateUniqueId(std::string prefix)
{
	long long timeNow = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	std::thread::id threadId = std::this_thread::get_id();
	std::stringstream ss;
	ss << prefix << "-" << std::hex << threadId << timeNow;
	return ss.str();
}


void utils::pingLookupServer(std::string_view serverName, int port) {
	CURL *handle = curl_easy_init();

	if(handle) {
		curl_easy_setopt(handle, CURLOPT_URL, "https://lookup.acraig.tech/ping");
		curl_easy_setopt(handle, CURLOPT_POST, 1);
		std::string postFields = std::string(serverName);

		if(port != 443) {
			postFields += ":" + std::to_string(port);
		}
		
		curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postFields.c_str());


		CURLcode res = curl_easy_perform(handle);

		if(res == CURLE_OK) {
			VERBOSE("Ping", "Ping sent successfully.");
		} else {
			WARNING("Ping", "Failed to ping server, curl code: {}", res);
		}
	} else {
		ERR("Ping", "Curl Handle is invalid");
	}

	// should be ran on a new thread
	std::this_thread::sleep_for(std::chrono::hours(PING_HOURS));
	utils::pingLookupServer(serverName, port);
}

std::string utils::hashPassword(std::string password)
{
	uint32_t t_cost = 2;            // 2-pass computation
    uint32_t m_cost = (1<<16);      // 64 mebibytes memory usage
    uint32_t parallelism = 2;       // number of threads and lanes

	std::string hashedPassword;
	hashedPassword.resize(HASHLEN*4);

	uint8_t salt[SALTLEN];
	RAND_bytes(salt, SALTLEN);
    ///memset( salt, 0x00, SALTLEN );

	size_t encodedSize = hashedPassword.size();
	int ret = argon2id_hash_encoded(t_cost, m_cost, parallelism, password.data(), password.size(), salt, SALTLEN, HASHLEN, hashedPassword.data(), encodedSize);
	
	if(ret != ARGON2_OK) {
		ERR("Hashing", "Failed to hash password! Error: {}", argon2_error_message(ret));
		hashedPassword = "";
	};

	hashedPassword.erase(std::find(hashedPassword.begin(), hashedPassword.end(), '\0'), hashedPassword.end());

	return hashedPassword;
}

bool utils::verifyPasswordHash(std::string password, std::string passwordHash)
{
	/*std::string testPassword = utils::hashPassword(password);

	if(testPassword.size() != passwordHash.size()) {
		WARNING("Hashing", "Provided passwords do not have the same size!");
		return false;
	} */

	int ret = argon2id_verify(passwordHash.c_str(), password.data(), password.length());

	return ret == ARGON2_OK;
}
