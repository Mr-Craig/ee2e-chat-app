#pragma once

#include <curl/curl.h>

#include <string>
#include <chrono>
#include <thread>
#include <sstream>

#define PING_HOURS (int)1

class utils
{
public:
	static std::string generateUniqueId(std::string prefix);
	static void pingLookupServer(std::string_view serverName, int port);
};