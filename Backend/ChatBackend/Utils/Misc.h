#pragma once
#include <string>
#include <chrono>
#include <thread>
#include <sstream>
#include <vector>
#include <tuple>

namespace utils 
{
	namespace misc
	{
		static std::string generateUniqueId(std::string prefix)
		{
			long long timeNow = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			std::thread::id threadId = std::this_thread::get_id();
			std::stringstream ss;
			ss << prefix << "-" << std::hex << threadId << timeNow;
			return ss.str();
		}

		static std::string getEnvironmentVariable(std::string env) {
			std::string ret;
#ifndef _WIN32
			const char* getenv = std::getenv(env.c_str());
			if (getenv != NULL)
				ret = getenv;
#else
			// use windows "safe" function
			char* buffer = nullptr;
			size_t bufferSz = 0;
			if (_dupenv_s(&buffer, &bufferSz, env.c_str()) == 0 && buffer != nullptr)
				ret.assign(buffer, bufferSz);
			free(buffer);
#endif
			return ret;
		}

		// https://www.geeksforgeeks.org/slicing-a-vector-in-c/
		template<class sType>
		static std::vector<sType> sliceVector(std::vector<sType>& array, int X, int Y)
		{
			// Starting and Ending iterators
			auto start = array.begin() + X;
			auto end = array.begin() + Y;

			// To store the sliced vector
			std::vector<sType> result(Y - X);

			// Copy vector using copy function()
			std::copy(start, end, result.begin());
			return result;
		}
	}
}