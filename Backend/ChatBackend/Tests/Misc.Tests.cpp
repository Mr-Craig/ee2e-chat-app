#include <gtest/gtest.h>
#include "../Utils/Misc.h"

TEST(Misc, RandomId) {
	std::string randomId = utils::misc::generateUniqueId("");
	std::string randomId2 = utils::misc::generateUniqueId("");

	EXPECT_NE(randomId, randomId2);
}

TEST(Misc, SliceVector) {
	std::vector<uint8_t> originalVector = { 0x00, 0x44, 0x55, 0x66, 0x66, 0x77, 0x9A };

	std::vector<uint8_t> slicedVector = utils::misc::sliceVector(originalVector, 1, originalVector.size());

	EXPECT_EQ(originalVector.size() - 1, slicedVector.size());
}