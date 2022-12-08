#include "Debug.h"

std::shared_ptr<debug> debug::debug_ptr = nullptr;

debug::debug()
{
	logfile.open("log.txt");

#ifdef __WINDOWS
	std::ios::sync_with_stdio(false);
#endif
}

debug::~debug()
{
	logfile.close();
}

std::shared_ptr<debug> debug::get()
{
	if (!debug_ptr) {
		debug_ptr = std::make_shared<debug>();
	}
	return debug_ptr;
}

#pragma warning( push )
#pragma warning( disable : 4996)
void debug::print(severity e, std::string category, std::string message)
{
	std::stringstream ss;

	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::string dateBuffer;
	dateBuffer.resize(32);
	size_t bufferSize = std::strftime(dateBuffer.data(), dateBuffer.size() * sizeof(char), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	dateBuffer.resize(bufferSize);

	std::cout << "[" << dateBuffer << "]" << "[\u001b[0m";

	ss << "[" << dateBuffer << "][";

	switch (e)
	{
	case FATAL:
		std::cout << "\u001b[30;1mFATAL";
		ss << "FATAL";
		break;
	case ERR:
		std::cout << "\u001b[31mERROR";
		ss << "ERROR";
		break;
	case VERBOSE:
		std::cout << "\u001b[37;1mVERBOSE";
		ss << "VERBOSE";
		break;
	case LOG:
		std::cout << "\u001b[32mLOG";
		ss << "LOG";
		break;
	case WARNING:
		std::cout << "\u001b[33;1mWARNING";
		ss << "WARNING";
		break;
	}

	std::cout << "\u001b[0m] " << category << ": " << message << "\n";
	ss << "] " << category << ": " << message << "\n";

	if (logfile.is_open()) {
		logfile << ss.str();

		if (flushCounter >= 10) {
			logfile.flush();
			flushCounter = 0;
		}

		flushCounter++;
	}

	// close on fatal
	if (e == FATAL) {
		std::exit(2);
	}
}
#pragma warning( pop ) 