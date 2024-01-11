// ChatBackend.cpp : Defines the entry point for the application.
//
#include <openssl/err.h>
#include <openssl/ec.h>
#include <vector>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <cppcodec/hex_default_lower.hpp>
#include <cppcodec/base64_default_url_unpadded.hpp>

#include "ChatBackend.h"

using namespace std;

int main(int argc, char** argv)
{
	std::shared_ptr<notifs> notifs = notifs::get();
	std::shared_ptr<websocket> ws = websocket::get();
	std::shared_ptr<db> db = db::get();
	
	std::thread lookupThread = utils::pinger::initPing(argc, argv);

	Handlers::registerHandlers();

	ws->getApp().run();
	
	return 0;
}
