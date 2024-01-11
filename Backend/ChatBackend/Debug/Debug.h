#pragma once

#include <memory>
#include <iostream>
#include <chrono>
#include <sstream>
#include <unordered_map>
#include <cstdlib>
#include <fstream>
#include <fmt/core.h>

#ifdef DEBUG
	#define VERBOSE(category, frmt, ...) debug::get()->print(debug::VERBOSE, category, fmt::format(frmt, ##__VA_ARGS__));
#else
	#define VERBOSE(category, frmt, ...) 0;
#endif
#define LOG(category, frmt, ...) debug::get()->print(debug::LOG, category, fmt::format(frmt, ##__VA_ARGS__));
#define WARNING(category, frmt, ...) debug::get()->print(debug::WARNING, category, fmt::format(frmt, ##__VA_ARGS__));
#define ERR(category, frmt, ...) debug::get()->print(debug::ERR, category, fmt::format(frmt, ##__VA_ARGS__));
#define FATAL(category, frmt, ...) debug::get()->print(debug::FATAL, category, fmt::format("\n      --------- Fatal Error Occured ---------      \n File: {}\n Line: {} \n Reason: {}", __FILE__, __LINE__, fmt::format(frmt, ##__VA_ARGS__)));

class debug
{
public:
	debug();
	~debug();

	static std::shared_ptr<debug> get();

	enum severity
	{
		VERBOSE,
		LOG,
		WARNING,
		ERR,
		FATAL
	};
	void print(severity e, std::string category, std::string message);
private:

	std::string getTimestamp();

	void flush(std::stringstream& s);

	std::ofstream logfile;

	struct enumPair {
		enumPair() {};
		enumPair(std::string c, std::string nc) : noColour(nc), colour(c) {};
		std::string noColour;
		std::string colour;
	};

	std::unordered_map<severity, enumPair> enumToString = {
		{VERBOSE, enumPair("\u001b[37;1mVERBOSE", "VERBOSE")},
		{LOG, enumPair("\u001b[32mLOG", "LOG")},
		{WARNING, enumPair("\u001b[33;1mWARNING", "WARNING")},
		{ERR, enumPair("\u001b[31mERROR", "ERROR")},
		{FATAL, enumPair("\u001b[30;1mFATAL", "FATAL")}
	};

	int flushCounter;
	static std::shared_ptr<debug> debug_ptr;
};
