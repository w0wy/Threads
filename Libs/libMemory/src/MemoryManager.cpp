#include <MemoryManager.h>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include <atomic>

namespace
{
	static boost::interprocess::mapped_region g_mapped_region;
	static constexpr uint32_t size_of_shared_mem = {64 * 1024};
	static uint32_t remaining_shared_mem = {64 * 1024};
}

namespace shmm
{

MemoryManager::MemoryManager()
{
	LOG_INFO_T(__func__, " Constructed.");
	initSharedMemory();
	expandPoolSize();
}

MemoryManager::~MemoryManager()
{
	LOG_INFO_T(__func__, " Destructed.");
	cleanUp();
	removeSharedMemory();
}

void MemoryManager::initSharedMemory()
{
	boost::interprocess::shared_memory_object shared_memory(
            boost::interprocess::create_only,
            SHARED_MEMORY,
            boost::interprocess::read_write);
    shared_memory.truncate(size_of_shared_mem);
    g_mapped_region = boost::interprocess::mapped_region(shared_memory, boost::interprocess::read_write);

    LOG_INFO_T(__func__, " Done for [shared_memory] object.");
    LOG_INFO_T(__func__, " Actual size reserved for queues : "
    	<< size_of_shared_mem / 1000 << " kb / "
    	<< size_of_shared_mem << " bytes.");
}

void MemoryManager::removeSharedMemory()
{
	boost::interprocess::shared_memory_object::remove(SHARED_MEMORY);

	LOG_INFO_T(__func__, " Done for [shared_memory] object.");
}

void MemoryManager::expandPoolSize()
{
	size_t size_of_message = (sizeof(Message) + MAX_SIZE_OF_MSG_DATA);
	size_t size_of_queue = (sizeof(MessageQueue) + (MAX_NUM_OF_MSGS * size_of_message));

	MessageQueue * que = (MessageQueue*) malloc(size_of_queue);
	MessageQueue * tempPtr = que;

	for (int i = 0; i < MAX_NUM_OF_QUEUES; i++)
	{
		que->next = (MessageQueue *) malloc(size_of_queue);
		memcpy(((char *)g_mapped_region.get_address() + i * size_of_queue), tempPtr, size_of_queue);
		remaining_shared_mem -= size_of_queue;
		tempPtr = que->next;
		free(que);
		que = tempPtr;
	}
	free(tempPtr);

	LOG_INFO_T(__func__, " Done for [shared_memory] object.");
	LOG_INFO_T(__func__, " Actual size occupied by queues : "
		<< (MAX_NUM_OF_QUEUES * size_of_queue) / 1000 << " kb / "
		<< (MAX_NUM_OF_QUEUES * size_of_queue) << " bytes.");
	LOG_INFO_T(__func__, " Remaining shared_memory : "
		<< remaining_shared_mem / 1000 << " kb / "
		<< remaining_shared_mem << " bytes.");
}

void MemoryManager::cleanUp()
{

}

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
