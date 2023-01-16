#pragma once

#include <curl/curl.h>

#include <string>
#include <chrono>
#include <thread>
#include <sstream>

#define PING_HOURS (int)1
#define HASHLEN 32
#define SALTLEN 16

class utils
{
public:
	static std::string generateUniqueId(std::string prefix);
	static void pingLookupServer(std::string_view serverName, int port);
	static std::string hashPassword(std::string password);
	static bool verifyPasswordHash(std::string password, std::string passwordHash);
};