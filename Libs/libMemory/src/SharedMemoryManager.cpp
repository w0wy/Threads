#include <SharedMemoryManager.h>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include "MessageQueue.h"

namespace
{
	static boost::interprocess::mapped_region g_mapped_region;
}

namespace shmm
{

SharedMemoryManager::SharedMemoryManager()
{
	initSharedMemory();
	expandPoolSize();
}

SharedMemoryManager::~SharedMemoryManager()
{
	cleanUp();
	removeSharedMemory();
}

void SharedMemoryManager::initSharedMemory()
{
	boost::interprocess::shared_memory_object::remove(SHARED_MEMORY);
	boost::interprocess::shared_memory_object shared_memory(
            boost::interprocess::create_only,
            SHARED_MEMORY,
            boost::interprocess::read_write);
    shared_memory.truncate(64 * 1024);
    g_mapped_region = boost::interprocess::mapped_region(shared_memory, boost::interprocess::read_write);

    LOG_DEBUG("Shared memory object [ shared_memory ] has been created and mapped_region has been set.");
}

void SharedMemoryManager::removeSharedMemory()
{
	boost::interprocess::shared_memory_object::remove(SHARED_MEMORY);
	LOG_DEBUG("Shared memory object [ shared_memory ] has been removed.");
}

void SharedMemoryManager::expandPoolSize()
{
	size_t size_of_message = (sizeof(Message) + MAX_SIZE_OF_MSG_DATA);
	size_t size_of_queue = (sizeof(MessageQueue) + (MAX_NUM_OF_MSGS * size_of_message));

	MessageQueue * que = (MessageQueue*) malloc(size_of_queue);
	MessageQueue * tempPtr = que;
 //    que->messages_count = 0;
 //    que->front_msg = nullptr;
 //    que->rear_msg = nullptr;
 //    que->messages = nullptr;
 //    que->next = nullptr;

	//memcpy(region_.get_address(), que, size_of_queue);
	// add_queue(region_.get_address());

	for (int i = 0; i < MAX_NUM_OF_QUEUES; i++)
	{
		que->next = (MessageQueue *) malloc(size_of_queue);
		que = que->next;
	}

	que->next = nullptr;

	memcpy(g_mapped_region.get_address(), tempPtr, MAX_NUM_OF_QUEUES * size_of_queue);

	free(tempPtr);
}

void SharedMemoryManager::cleanUp()
{}

}  // namespace memhelp

// memhelp::MemoryManager gMemoryManager;

// void* operator new (size_t size)
// {
//     return gMemoryManager.allocate(size);
// }

// void* operator new[] (size_t size)
// {
//     return gMemoryManager.allocate(size);
// }

// void operator delete (void* pointerToDelete)
// {
//     gMemoryManager.free(pointerToDelete);
// }

// void operator delete[] (void* arrayToDelete)
// {
//     gMemoryManager.free(arrayToDelete);
// }
