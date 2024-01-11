#include <gtest/gtest.h>
#include <cppcodec/base64_default_url_unpadded.hpp>
#include "../Notifications/Vapid.h"

#define PRIVATE_KEY "D67D7F5895BB724B550200BE1DF494C763018E6C6A588A74B7ED4D997778C946"
#define PUBLIC_KEY "049A082BA570E17F20E9B675E10310B517AA8A326A42B09CEDAEA608AF877463E2993AEF417F4C8863F4A5414F2EF48A047B15B15B5BE1A3C398334D3AFEF68997"
#define TEST_AUD "https://testing.com"

TEST(VapidTest, TestJWT) {
	std::string testAud = TEST_AUD;

	utils::crypto::EcKeyPtr EcKey = std::make_shared<utils::crypto::EcKey>(PUBLIC_KEY, PRIVATE_KEY);

	Vapid vapidTest(testAud, EcKey);

	std::string JWTString = vapidTest.generateJWT();

	std::string header = JWTString.substr(0, JWTString.find('.'));
	JWTString.erase(0, JWTString.find('.') + 1);

	std::string body = JWTString.substr(0, JWTString.find('.'));
	JWTString.erase(0, JWTString.find('.') + 1);

	std::string signature = JWTString.substr(0, JWTString.find('.'));

	std::string digestString = header + "." + body;

	std::vector<uint8_t> digestBytes(digestString.begin(), digestString.end());
	std::vector<uint8_t> digest = utils::crypto::HashSHA256(digestBytes);
	std::vector<uint8_t> sig = cppcodec::base64_url_unpadded::decode(signature);

	const uint8_t* sigp = sig.data();
	ECDSA_SIG* sigObj = ECDSA_SIG_new();

	BIGNUM* r = BN_new(), * s = BN_new();

	// signature is r and s values concatenated, both 32-byte integers.
	BN_bin2bn(sig.data(), 32, r);
	BN_bin2bn(&sig.data()[32], 32, s);

	ECDSA_SIG_set0(sigObj, r, s);

	int result = ECDSA_do_verify(digest.data(), digest.size(), sigObj, EcKey->getRawPtr());

	EXPECT_EQ(result, 1);
}