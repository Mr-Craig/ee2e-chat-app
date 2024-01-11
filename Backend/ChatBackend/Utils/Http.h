#pragma once
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <unordered_map>

namespace utils
{
	namespace http
	{
		static size_t noop_cb(void* ptr, size_t size, size_t nmemb, void* data) {
			return size * nmemb;
		}
		typedef std::unordered_map<std::string/*header name*/, std::string /*header content*/> HeaderMap;
		static long POST(std::string& endpoint, std::vector<uint8_t>& body, HeaderMap headers = HeaderMap())
		{
			CURL* handle = curl_easy_init();

			if (!handle)
				return 0;

			curl_easy_setopt(handle, CURLOPT_URL, endpoint.c_str());
			curl_easy_setopt(handle, CURLOPT_POST, 1);

			// write the data to nothing, since we don't care.
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &noop_cb);

			curl_easy_setopt(handle, CURLOPT_POSTFIELDS, body.data());
			curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, body.size());

			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);

			//FIDDLER
			//curl_easy_setopt(handle, CURLOPT_PROXY, "127.0.0.1:8888");

			struct curl_slist* cHeaders = NULL;

			for (auto header : headers) {
				cHeaders = curl_slist_append(cHeaders, fmt::format("{}: {}", header.first, header.second).c_str());
			}

			curl_easy_setopt(handle, CURLOPT_HTTPHEADER, cHeaders);

			CURLcode res = curl_easy_perform(handle);

			if (res != CURLE_OK)
				return 0;

			long httpCode = 0;
			curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &httpCode);

			return httpCode;
		}

		static long POST(std::string& endpoint, std::string& body, HeaderMap headers = HeaderMap())
		{
			std::vector<uint8_t> bodyBytes(body.begin(), body.end());
			return POST(endpoint, bodyBytes, headers);
		}
	}
}