#include "Utils.h"

std::string utils::generateUniqueId(std::string prefix)
{
	long long timeNow = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	std::thread::id threadId = std::this_thread::get_id();
	std::stringstream ss;
	ss << prefix << "-" << std::hex << threadId << timeNow;
	return ss.str();
}
