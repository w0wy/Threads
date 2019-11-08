#include "gtest/gtest.h"
#include "MemoryManager.h"
#include "MessageQueue.h"

struct SomeExampleTestFixture : public testing::Test
{
	void SetUp(){};
	void TearDown(){};
};

TEST_F(SomeExampleTestFixture, SomeExampleTestCase)
{
	EXPECT_EQ(MemoryManager::getInstance().allocate<MessageQueue>(), nullptr);
}


TEST(SomeExampleSoloTest, SomeExampleTestCase)
{
	EXPECT_NE(MemoryManager::getInstance().allocate<MessageQueue>(), nullptr);
}