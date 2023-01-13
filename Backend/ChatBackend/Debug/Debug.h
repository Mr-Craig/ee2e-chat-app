#pragma once

#include <memory>
#include <iostream>
#include <chrono>
#include <sstream>
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
	std::ofstream logfile;

	int flushCounter;
	static std::shared_ptr<debug> debug_ptr;
};
