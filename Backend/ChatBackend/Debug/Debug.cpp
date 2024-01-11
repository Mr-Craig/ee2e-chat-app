#include "Debug.h"

std::shared_ptr<debug> debug::debug_ptr = nullptr;

debug::debug()
{
	logfile.open("log.txt");
	std::ios::sync_with_stdio(true);
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

void debug::print(severity e, std::string category, std::string message)
{
	std::stringstream ss;

	std::string currentDate = getTimestamp();
	std::cout << "[" << currentDate << "]" << "[\u001b[0m";
	ss << "[" << currentDate << "][";

	std::cout << enumToString[e].colour;
	ss << enumToString[e].noColour;

	std::cout << "\u001b[0m] " << category << " - " << message << "\n";
	ss << "] " << category << " - " << message << "\n";

	flush(ss);

	if (e == FATAL) std::exit(2);
}

void debug::flush(std::stringstream& s)
{
	if (!logfile.is_open())
		return;

	logfile << s.str();

	if (flushCounter >= 10) {
		logfile.flush();
		flushCounter = 0;
	}

	flushCounter++;
}

std::string debug::getTimestamp()
{
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::string dateBuffer;
	dateBuffer.resize(32);

#ifdef WIN32
	// windows safe
	struct tm newtime;
	localtime_s(&newtime, &now);
	size_t bufferSize = std::strftime(dateBuffer.data(), dateBuffer.size() * sizeof(char), "%Y-%m-%d %H:%M:%S", &newtime);
#else
	size_t bufferSize = std::strftime(dateBuffer.data(), dateBuffer.size() * sizeof(char), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
#endif

	dateBuffer.resize(bufferSize);

	return dateBuffer;
}