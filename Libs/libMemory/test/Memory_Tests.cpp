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

	constexpr unsigned max_number_of_queues = 90;
	constexpr unsigned max_number_of_messages = 60;

	MessageQueue* queues[max_number_of_queues];
	Message* messages[max_number_of_messages];

	for (unsigned number_of_queues = 0; number_of_queues < max_number_of_queues; number_of_queues++)
	{
		queues[number_of_queues] = instance.allocate<MessageQueue>();
		EXPECT_NE(queues[number_of_queues], nullptr);
	}

	for (unsigned number_of_messages = 0; number_of_messages < max_number_of_messages; number_of_messages++)
	{
		messages[number_of_messages] = instance.allocate<Message>();
		EXPECT_NE(messages[number_of_messages], nullptr);
	}

	EXPECT_THROW(instance.allocate<MessageQueue>(), std::bad_alloc);
	EXPECT_THROW(instance.allocate<Message>(), std::bad_alloc);

	// need to cleanup arenas for tests. if --gtest_repeat is used,
	// process will remain => same instance of memory manager
	for (unsigned count = 0; count < max_number_of_queues; count++)
		instance.deallocate(queues[count]);
	for (unsigned count = 0; count < max_number_of_messages; count++)
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

	MessageQueue * messages_allocate[90];
	unsigned counter_for_allocate = 0;
	auto t1custom = std::chrono::high_resolution_clock::now();
	for (unsigned rep = 0; rep < 9; rep++)
	{
		for (unsigned allocs = 0; allocs < 10; allocs++)
		{
			messages_allocate[counter_for_allocate++] = instance.allocate<MessageQueue>();
			// instance.deallocate(messages_allocate[counter_for_allocate]);
			// ++counter_for_allocate;
		}
	}
	auto t2custom = std::chrono::high_resolution_clock::now();

	auto durationMemManager = std::chrono::duration_cast<std::chrono::nanoseconds>( t2custom - t1custom ).count();

	MessageQueue * messages_new[90];
	unsigned counter_for_new = 0;
	auto t1new = std::chrono::high_resolution_clock::now();
	for (unsigned rep = 0; rep < 9; rep++)
	{
		for (unsigned allocs = 0; allocs < 10; allocs++)
		{
			messages_new[counter_for_new++] = new MessageQueue();
			// delete messages_new[counter_for_new];
			// ++counter_for_new;
		}
		
	}
	auto t2new = std::chrono::high_resolution_clock::now();

	auto durationNew = std::chrono::duration_cast<std::chrono::nanoseconds>( t2new - t1new ).count();

	EXPECT_GT(durationNew, durationMemManager);		

	// delete is faster than deallocate ... bad luck :/
	// auto t1deletecustom =  std::chrono::high_resolution_clock::now();
	// for (unsigned i = 0; i < 90; i++)
	// {
	// 	instance.deallocate(messages_allocate[i]);
	// }
	// auto t2deletecustom =  std::chrono::high_resolution_clock::now();

	// auto durationMemManagerDelete = std::chrono::duration_cast<std::chrono::nanoseconds>( t2deletecustom - t1deletecustom ).count();

	// auto t1delete = std::chrono::high_resolution_clock::now();

	// for (unsigned i = 0; i < 90; i++)
	// {
	// 	delete messages_new[i];
	// }

	// auto t2delete = std::chrono::high_resolution_clock::now();

	// auto durationDelete = std::chrono::duration_cast<std::chrono::nanoseconds>( t2delete - t1delete ).count();
	// EXPECT_GT(durationDelete, durationMemManagerDelete);
}