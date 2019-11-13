#include "gtest/gtest.h"
#include "MemoryManager.h"
#include "MessageQueue.h"

#include <chrono>

struct MemoryManagerTests : public testing::Test
{
	void SetUp(){};
	void TearDown(){};
};

TEST_F(MemoryManagerTests, ShouldReturnSameInstance)
{
	auto& instance = MemoryManager::getInstance();

	MemoryManager* ptr = &instance;
	EXPECT_NE(ptr, nullptr);

	EXPECT_EQ(&MemoryManager::getInstance(), &instance);
}

TEST_F(MemoryManagerTests, AllArenasWithAllPoolsShouldBeAvailable)
{
	auto& instance = MemoryManager::getInstance();

	MessageQueue* queues[90];
	Message* messages[60];

	unsigned counter_for_que = 0;
	for (unsigned msg_que_arenas = 0; msg_que_arenas < 3; msg_que_arenas++)
	{
		for (unsigned msg_que_pools = 0; msg_que_pools < 3; msg_que_pools++)
		{
			for (unsigned msg_que = 0; msg_que < 10; msg_que++)
			{
				queues[counter_for_que] = instance.allocate<MessageQueue>();
				EXPECT_NE(queues[counter_for_que++], nullptr);
			}
		}
	}

	unsigned counter_for_message = 0;
	for (unsigned msg_arenas = 0; msg_arenas < 2; msg_arenas++)
	{
		for (unsigned msg_pools = 0; msg_pools < 3; msg_pools++)
		{
			for(unsigned msg = 0; msg < 10; msg++)
			{
				messages[counter_for_message] = instance.allocate<Message>();
				EXPECT_NE(messages[counter_for_message++], nullptr);
			}
		}
	}

	EXPECT_EQ(instance.allocate<MessageQueue>(), nullptr);
	EXPECT_EQ(instance.allocate<Message>(), nullptr);

	// need to cleanup arenas for tests. if --gtest_repeat is used,
	// process will remain => same instance of memory manager
	for (unsigned count = 0; count < 90; count++)
		instance.deallocate(queues[count]);
	for (unsigned count = 0; count < 60; count++)
		instance.deallocate(messages[count]);
}

TEST_F(MemoryManagerTests, ShouldHaveCorrectValues)
{
	auto& instance = MemoryManager::getInstance();
	MessageQueue* queues[3];

	std::string str("testString");
	const char* data_for_messages = str.c_str();
	for (unsigned msg_que = 0; msg_que < 3; msg_que++)
	{
		queues[msg_que] = instance.allocate<MessageQueue>();
		queues[msg_que]->messages_count=1;
		queues[msg_que]->data = instance.allocate<Message>();
		// TODO add allocate<base types> -> sfinae in memory manager to check if "SIZE()" function exists
		// specialize allocate and deallocate functions for 
		// maybe check std::is_trivial, std::is_fundamental
		queues[msg_que]->data->data = (char *)malloc (10);
		memcpy(queues[msg_que]->data->data, data_for_messages, 10);
		queues[msg_que]->data->size_of_data = 10;
		queues[msg_que]->size_of_data = queues[msg_que]->data->size_of_data;
		queues[msg_que]->front_msg = queues[msg_que]->data;
		queues[msg_que]->rear_msg = queues[msg_que]->data;
	}

	for (unsigned msg_que = 0; msg_que < 3; msg_que++)
	{
		ASSERT_NE(queues[msg_que], nullptr);
		ASSERT_NE(queues[msg_que]->data, nullptr);
		ASSERT_NE(queues[msg_que]->data->data, nullptr);
		
		EXPECT_EQ(queues[msg_que]->messages_count, 1);
		
		EXPECT_EQ(*queues[msg_que]->data->data, *data_for_messages);
		EXPECT_EQ(queues[msg_que]->data->size_of_data, 10);
		EXPECT_EQ(queues[msg_que]->size_of_data, 10);
		EXPECT_EQ(queues[msg_que]->front_msg, queues[msg_que]->data);
		EXPECT_EQ(queues[msg_que]->rear_msg, queues[msg_que]->data);
	}

	for (unsigned msg_que = 0; msg_que < 3; msg_que++)
	{	
		// TODO move these to deallocate in memory manager or message queue impl
		queues[msg_que]->messages_count = 0;
		queues[msg_que]->data->size_of_data = 0;
		queues[msg_que]->size_of_data = 0;
		queues[msg_que]->front_msg = nullptr;
		queues[msg_que]->rear_msg = nullptr;
		// END of TODO
		free(queues[msg_que]->data->data);
		instance.deallocate(queues[msg_que]->data);
		instance.deallocate(queues[msg_que]);
	}
}

TEST_F(MemoryManagerTests, ShouldPerformFaster)
{
	auto& instance = MemoryManager::getInstance();

	auto t1custom = std::chrono::high_resolution_clock::now();
	for (unsigned rep = 0; rep < 5; rep++)
	{
		for (unsigned allocs = 0; allocs < 90; allocs++)
		{
			auto msg = instance.allocate<MessageQueue>();
			instance.deallocate(msg);
		}
	}
	auto t2custom = std::chrono::high_resolution_clock::now();

	auto durationMemManager = std::chrono::duration_cast<std::chrono::nanoseconds>( t2custom - t1custom ).count();

	auto t1new = std::chrono::high_resolution_clock::now();
	for (unsigned rep = 0; rep < 5; rep++)
	{
		for (unsigned allocs = 0; allocs < 90; allocs++)
		{
			auto msg = new MessageQueue();
			delete msg;
		}
		
	}
	auto t2new = std::chrono::high_resolution_clock::now();

	auto durationNew = std::chrono::duration_cast<std::chrono::nanoseconds>( t2new - t1new ).count();

	EXPECT_GT(durationNew, durationMemManager);
}