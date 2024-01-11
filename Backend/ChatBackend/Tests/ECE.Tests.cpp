#include <gtest/gtest.h>
#include <ece.h>
#include <cppcodec/base64_default_url_unpadded.hpp>

#define RECIEVER_KEY "BOLcHOg4ajSHR6BjbSBeX_6aXjMu1V5RrUYXqyV_FqtQSd8RzdU1gkMv1DlRPDIUtFK6Nd16Jql0eSzyZh4V2uc"
#define SALT "4CQCKEyyOT_LysC17rsMXQ"
#define AUTH_SECRET "r9kcFt8-4Q6MnMjJHqJoSQ"
#define PLAINTEXT "Hello, world!"
#define PADDING 0

TEST(ECE, EncryptionParameters) {
	std::string pt = PLAINTEXT;
	size_t paddingLength = PADDING;
	std::vector<uint8_t> rawPublicKey = cppcodec::base64_url_unpadded::decode(RECIEVER_KEY);
	std::vector<uint8_t> authSecret = cppcodec::base64_url_unpadded::decode(AUTH_SECRET);
	std::vector<uint8_t> cipherText(ece_aesgcm_ciphertext_max_length(ECE_WEBPUSH_DEFAULT_RS, paddingLength, pt.size()));

	// these will be populated by the ecec library
	std::vector<uint8_t> salt(ECE_SALT_LENGTH);
	std::vector<uint8_t> rawSenderPublicKey(ECE_WEBPUSH_PUBLIC_KEY_LENGTH);

	size_t cipherTextLength = cipherText.size();

	int err = ece_webpush_aesgcm_encrypt(rawPublicKey.data(), rawPublicKey.size(), authSecret.data(), authSecret.size(), ECE_WEBPUSH_DEFAULT_RS, paddingLength, (uint8_t*)pt.data(), pt.size(), salt.data(), salt.size(), rawSenderPublicKey.data(), rawSenderPublicKey.size(), cipherText.data(), &cipherTextLength);

	EXPECT_TRUE(err == ECE_OK);
}