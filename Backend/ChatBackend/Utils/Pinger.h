#pragma once
#include <string>
#include <chrono>
#include <thread>
#include "../Debug/Debug.h"
#include "../Socket/Socket.h"
#include "Http.h"
#include <sstream>
#include <vector>
#include <tuple>

#define LOOKUP_SERVER "https://lookup.acraig.tech/ping"
namespace utils
{
	namespace pinger
	{

		class helper
		{
		public:
			helper(std::string sn, int po) : serverName(sn), port(po) {};

			void pingThread() {
				while (true) {
					doPing();
					std::this_thread::sleep_for(std::chrono::hours(1));
				}
			}
		private:
			void doPing() {
				std::string postFields = std::string(serverName);
				if (port != 443) {
					postFields += ":" + std::to_string(port);
				}

				std::string lookupServer = LOOKUP_SERVER;
				long httpCode = utils::http::POST(lookupServer, postFields);

				if (httpCode != 200) {
					WARNING("Ping", "Failed to ping lookup server, http code: {}", httpCode);
				}
				else {
					LOG("Ping", "Successfully Pinged Lookup Server");
				}
			}

			const std::string serverName;
			const int port;
		};

		static std::thread initPing(int argc, char** argv)
		{
			if (argc < 3 || std::string(argv[1]) != "--ip") {
				WARNING("Ping", "Invalid Arguments, will not ping server.");
				return std::thread();
			}
			
			std::string serverName = std::string(argv[2]);
			// Start Ping

			int port = websocket::get()->getPort();

			if (argc >= 4 && std::string(argv[3]) == "--no-port") 
				port = 443;

			helper* help = new helper(serverName, port);

			VERBOSE("Ping", "Server Name: {}:{}", serverName, port);

			return std::thread(&helper::pingThread, help);
		}
	}
}