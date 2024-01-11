#include "../Utils/Argon2.h"
#include <gtest/gtest.h>

#define PASSWORD "password"
#define INCORRECT_PASSWORD "assword"

TEST(Argon2, HashPassword) {
	std::string hash = utils::argon2::hashPassword(PASSWORD);

	EXPECT_NE(hash.size(), 0);
}

TEST(Argon2, ComparePassword) {
	std::string hash = utils::argon2::hashPassword(PASSWORD);

	EXPECT_TRUE(utils::argon2::verifyPasswordHash(PASSWORD, hash));
	EXPECT_FALSE(utils::argon2::verifyPasswordHash(INCORRECT_PASSWORD, hash));
}