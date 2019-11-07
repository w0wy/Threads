#ifndef MEMORY_MANAGER_H_
#define MEMORY_MANAGER_H_

#define SHARED_MEMORY "shared_memory"
#define DEFAULT_NUM_OF_ELEMENTS 10

#include <sys/types.h>
#include <atomic>

#include "Logger.h"

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

namespace shmm
{

struct MemPool
{
	size_t size_in_bytes;
	size_t remaining_blocks;
	char* pool;
	char* free_slot;
};

struct MemArena
{
	uint32_t pools_in_use;
	size_t memory_block_size;
	MemPool pools[3];

	template <typename T>
	char* allocate(size_t sz)
	{
		for (unsigned i = 0; i < 3; i++)
		{
			if (pools[i].remaining_blocks >= 1)
			{
				pools[i].remaining_blocks--;
				char * toReturn = pools[i].free_slot;
				if (memory_block_size > sz)
				{
					unsigned diff = memory_block_size - sz;
					pools[i].free_slot = (char *)((T*)pools[i].free_slot)->next + diff;
				}
				else
				{
					pools[i].free_slot = (char *)((T*)pools[i].free_slot)->next;
				}

				return toReturn;
			}
		}
	}

	template <typename T>
	bool deallocate(void * ptrToDelete, size_t sz)
	{
		for (unsigned i = 0; i < 3; i++)
		{
			if ((char*)ptrToDelete >= pools[i].pool &&
				(char*)ptrToDelete <= pools[i].pool + pools[i].size_in_bytes)
			{
				char * last_free_slot = pools[i].free_slot;
				pools[i].free_slot = (char*) ptrToDelete;
				((T*)pools[i].free_slot)->next = (T*)last_free_slot;
				return true;
			}
		}
		return false;
	}
};

struct SharedMem
{
	boost::interprocess::mapped_region mapped_region;
	char* last_known_addr;
};

class MemoryManager
{
public:
	static MemoryManager& getInstance();

	template<typename T>
	void* allocate()
	{
		for(unsigned i = 0; i < 5 /*arenas*/; i++)
		{
			if (memory_arenas[i].memory_block_size >= T::SIZE())
			{
				return (void*)memory_arenas[i].allocate<T>(T::SIZE());
			}
		}
	}

	template <typename T>
	void deallocate(void* ptrToDelete)
	{
		for (unsigned i = 0; i < 5; i++)
		{
			if (memory_arenas[i].memory_block_size >= T::SIZE() &&
				memory_arenas[i].deallocate<T>(ptrToDelete, T::SIZE()));
			{
				return;
			}
		}	
	}
private:
	MemoryManager();
	virtual ~MemoryManager();
	MemoryManager(const MemoryManager&) = delete;
	MemoryManager& operator=(const MemoryManager&) = delete;

	void expandSharedMemory();
	void initSharedMemory();

	template <typename T>
	void expandArenas(uint32_t begin, uint32_t end)
	{
		unsigned size_in_bytes = DEFAULT_NUM_OF_ELEMENTS * T::SIZE();

		for (unsigned i = begin; i < end; i++)
		{
			for (unsigned j = 0; j < 3; j++)
			{
				memory_arenas[i].pools[j].size_in_bytes = size_in_bytes;
				memory_arenas[i].pools[j].remaining_blocks = DEFAULT_NUM_OF_ELEMENTS;
				memory_arenas[i].pools[j].pool = (char *) malloc(memory_arenas[i].pools[j].size_in_bytes);
				memory_arenas[i].pools[j].free_slot = memory_arenas[i].pools[j].pool;

				T* free_slot = (T*)memory_arenas[i].pools[j].free_slot;
				free_slot->next = (T*)memory_arenas[i].pools[j].free_slot + T::SIZE();
			}
		}

		LOG_INFO_T(__func__, "Done for arenas [" << begin << " to " << end << ") with blocks of size : "
			<< size_in_bytes / 1000 << " kb / "
			<< size_in_bytes << " bytes.");
	}

	void cleanUp();
	void removeSharedMemory();

	SharedMem shared_memory_region;
	MemArena memory_arenas[5];
};

}  // namespace memhelp

#endif  // MEMORY_MANAGER_H_