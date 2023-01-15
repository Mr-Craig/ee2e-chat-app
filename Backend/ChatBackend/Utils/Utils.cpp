#include <future>
#include <chrono>
#include <thread>

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

	static_cast<void>(std::async(std::launch::async, [serverName, port]{
		std::this_thread::sleep_for(std::chrono::hours(PING_HOURS));
		utils::pingLookupServer(serverName, port);
	}));
}