#include <gtest/gtest.h>
#include <cppcodec/hex_default_lower.hpp>
#include "../Utils/Crypto.h"

#define NUM_BYTES 32

TEST(Crypto, RandomBytes) {
	std::vector<uint8_t> randomBytes = utils::crypto::randomBytes(NUM_BYTES);
	std::vector<uint8_t> randomBytes2 = utils::crypto::randomBytes(NUM_BYTES);

	EXPECT_EQ(randomBytes.size(), NUM_BYTES) << "Random bytes incorrect size";
	EXPECT_NE(randomBytes, randomBytes2) << "Both are the same";
}

#define HASH "e806a291cfc3e61f83b98d344ee57e3e8933cccece4fb45e1481f1f560e70eb1"
#define UNHASHED "Testing"

TEST(Crypto, Hashing) {
	std::string testString = UNHASHED;
	std::vector<uint8_t> testBytes(testString.begin(), testString.end());

	std::vector<uint8_t> hash = utils::crypto::HashSHA256(testBytes);

	std::string hashHex = cppcodec::hex_lower::encode(hash);

	EXPECT_EQ(hash.size(), 32) << "Hash is not 32 bytes";
	EXPECT_EQ(hashHex, HASH) << "Hash doesn't equal test value";
}

TEST(Crypto, EcKey) {
	utils::crypto::EcKeyPtr EcKey = std::make_shared<utils::crypto::EcKey>();

	EXPECT_TRUE(EcKey->isValid());

	std::string privateHex = EcKey->getPublicKeyHex();
	std::string publicHex = EcKey->getPublicKeyHex();

	utils::crypto::EcKeyPtr EcKeyFromHex = std::make_shared<utils::crypto::EcKey>(publicHex, privateHex);

	EXPECT_TRUE(EcKey->isValid());
}