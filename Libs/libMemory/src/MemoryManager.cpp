#include <MemoryManager.h>
#include <MessageQueue.h>
#include <Message.h>

#include <atomic>

namespace
{
	static constexpr uint32_t size_of_shared_mem = {512 * 1024};
	static uint32_t remaining_shared_mem = {512 * 1024};
}

namespace shmm
{

MemoryManager::MemoryManager()
{
	LOG_INFO_T(__func__, " Constructed.");

	expandSharedMemory();
	expandArenas<MessageQueue>(0, 3);
	expandArenas<Message>(3, 5);
	initSharedMemory();
}

MemoryManager::~MemoryManager()
{
	LOG_INFO_T(__func__, " Destructed.");

	cleanUp();
	removeSharedMemory();
}

void MemoryManager::expandSharedMemory()
{
	removeSharedMemory();
	boost::interprocess::shared_memory_object shared_memory(
            boost::interprocess::create_only,
            SHARED_MEMORY,
            boost::interprocess::read_write);
    shared_memory.truncate(size_of_shared_mem);
    shared_memory_region.mapped_region = boost::interprocess::mapped_region(
    	shared_memory, boost::interprocess::read_write);
    shared_memory_region.last_known_addr = (char *)shared_memory_region.mapped_region.get_address();

    LOG_INFO_T(__func__, " Done for [shared_memory] object.");
    LOG_INFO_T(__func__, " Actual size of shared memory reserved : "
    	<< size_of_shared_mem / 1000 << " kb / "
    	<< size_of_shared_mem << " bytes.");
}

void MemoryManager::initSharedMemory()
{
	for (unsigned i = 0; i < 5; i++)
	{
		for (unsigned j = 0; j < 3; j++)
		{
			memcpy(shared_memory_region.last_known_addr, memory_arenas[i].pools[j].pool, memory_arenas[i].pools[j].size_in_bytes);
			shared_memory_region.last_known_addr += memory_arenas[i].pools[j].size_in_bytes;
			remaining_shared_mem -= memory_arenas[i].pools[j].size_in_bytes;
		}
	}

    LOG_INFO_T(__func__, " Actual size of shared memory remaining : "
    	<< remaining_shared_mem / 1000 << " kb / "
    	<< remaining_shared_mem << " bytes.");
}

void MemoryManager::removeSharedMemory()
{
	boost::interprocess::shared_memory_object::remove(SHARED_MEMORY);

	LOG_INFO_T(__func__, " Done for [shared_memory] object.");
}

void MemoryManager::cleanUp()
{
	for (unsigned i = 0; i < 5; i++)
	{
		for (unsigned j = 0; j < 3; j++)
	 		free(memory_arenas[i].pools[j].pool);
	}

	LOG_INFO_T(__func__, " Done.");
}

MemoryManager& MemoryManager::getInstance()
{
	static MemoryManager instance;
	return instance;
}

}  // namespace memhelp
