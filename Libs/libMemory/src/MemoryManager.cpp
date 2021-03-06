#include "MemoryManager.h"
#include "Logger.h"

namespace
{
	static constexpr uint32_t size_of_shared_mem = {512 * 1024};
	static uint32_t remaining_shared_mem = {512 * 1024};
}

MemoryManager::MemoryManager()
{
	LOG_INFO_T(__func__, " Constructed.");

	expandSharedMemory();
	expandArenas(0, 3, 40);
	expandArenas(3, 5, 16);
	initSharedMemory();
}

MemoryManager::~MemoryManager()
{
	LOG_INFO_T(__func__, " Destructed.");

	cleanUp();
	removeSharedMemory();
}

void MemoryManager::expandArenas(uint32_t begin, uint32_t end, size_t sz)
{
	unsigned size_in_bytes = DEFAULT_NUM_OF_ELEMENTS * sz + 1 + sizeof(PoolHeader);
	for (unsigned i = begin; i < end; i++)
	{
		memory_arenas[i].pools_in_use = 0;
		memory_arenas[i].memory_block_size = sz;
		memory_arenas[i].free_slot_pool = 0;

		for (unsigned j = 0; j < POOLS; j++)
		{
			memory_arenas[i].pools[j].size_in_bytes = size_in_bytes;
			memory_arenas[i].pools[j].remaining_blocks = DEFAULT_NUM_OF_ELEMENTS;
			memory_arenas[i].pools[j].pool = (char *) malloc(memory_arenas[i].pools[j].size_in_bytes);

			char b = '#';
			memcpy(memory_arenas[i].pools[j].pool, &b, 1);

			PoolHeader header{(char *)&memory_arenas[i], (char *)&memory_arenas[i].pools[j]};

			char* header_ref_pos = memory_arenas[i].pools[j].pool + 1;
			memcpy(header_ref_pos, &header, sizeof(PoolHeader));

			memory_arenas[i].pools[j].free_slot = memory_arenas[i].pools[j].pool + sizeof(PoolHeader) + 1;
			memory_arenas[i].pools_in_use++;

			MemBlock* head = (MemBlock*)(memory_arenas[i].pools[j].free_slot);
			for (unsigned k = 0; k < DEFAULT_NUM_OF_ELEMENTS ; k ++)
			{
				head->next = (MemBlock*)((char *)head + sz);
				head = (MemBlock*)head->next;
			}

			//head->next = nullptr;

			LOG_DEBUG_T(__func__, "Arena [" << i << "].pools[" << j << "] has size of : " << size_in_bytes
				<< " bytes with " << DEFAULT_NUM_OF_ELEMENTS << " blocks of " << sz << " bytes. Starting address : 0x"
				<< (uintptr_t)memory_arenas[i].pools[j].pool << " free slot : 0x" << (uintptr_t)memory_arenas[i].pools[j].free_slot
				<< " and end address : 0x" << (uintptr_t) (memory_arenas[i].pools[j].pool + size_in_bytes) << " .");
		}
	}

	LOG_INFO_T(__func__, "Done for arenas [" << begin << " to " << end << ") with full size of : "
		<< size_in_bytes / 1000 << " kb / "
		<< size_in_bytes << " bytes and blocks of size : "
		<< sz);
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
	for (unsigned i = 0; i < ARENAS; i++)
	{
		for (unsigned j = 0; j < POOLS; j++)
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
	for (unsigned i = 0; i < ARENAS; i++)
	{
		for (unsigned j = 0; j < POOLS; j++)
	 		free(memory_arenas[i].pools[j].pool);
	}

	LOG_INFO_T(__func__, " Done.");
}
