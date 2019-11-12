#ifndef MEMORY_MANAGER_H_
#define MEMORY_MANAGER_H_

#define SHARED_MEMORY "shared_memory"
#define DEFAULT_NUM_OF_ELEMENTS 10

#include <sys/types.h>
#include <atomic>
#include <utility>

#include "Logger.h"

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

struct MemBlock
{
	MemBlock * next;
};

struct MemPool
{
	size_t size_in_bytes;
	size_t remaining_blocks;
	size_t contiguous_free;
	char* pool;
	char* free_slot;
};

struct MemArena
{
	uint32_t pools_in_use;
	size_t memory_block_size;
	MemPool pools[3];

	char* allocate(size_t sz)
	{
		for (unsigned i = 0; i < 3; i++)
		{
			if (pools[i].remaining_blocks >= 1)
			{
				pools[i].remaining_blocks--;
				char * toReturn = pools[i].free_slot;

				if (pools[i].remaining_blocks == 0)
				{
					pools[i].free_slot = nullptr;
					return toReturn;
				}

				pools[i].free_slot = (char *) ((MemBlock*)pools[i].free_slot)->next;
				return toReturn;
			}
		}

		return nullptr;
	}

	bool deallocate(void * ptrToDelete)
	{
		for (unsigned i = 0; i < 3; i++)
		{
			if ((char*)ptrToDelete >= pools[i].pool &&
				(char*)ptrToDelete <= (pools[i].pool + pools[i].size_in_bytes))
			{
				char * last_free_slot = pools[i].free_slot;
				pools[i].free_slot = (char*) ptrToDelete;
				
				if (last_free_slot != nullptr)
					((MemBlock*)pools[i].free_slot)->next = (MemBlock*)last_free_slot;
				else
					((MemBlock*)pools[i].free_slot)->next = nullptr;

				pools[i].remaining_blocks++;
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
	T* allocate()
	{
		for(unsigned i = 0; i < 5; i++)
		{
			if (memory_arenas[i].memory_block_size >= sizeof(T))
			{
				T* p_to_return = (T*) memory_arenas[i].allocate(sizeof(T));
				if (p_to_return != nullptr)
					return p_to_return;
			}
		}

		return nullptr;
	}

	template <typename T>
	void deallocate(T* ptrToDelete)
	{
		for (unsigned i = 0; i < 5; i++)
		{
			if (memory_arenas[i].memory_block_size >= sizeof(T) &&
				memory_arenas[i].deallocate(ptrToDelete))
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

	void expandArenas(uint32_t begin, uint32_t end, size_t sz)
	{
		unsigned size_in_bytes = DEFAULT_NUM_OF_ELEMENTS * sz;
		for (unsigned i = begin; i < end; i++)
		{
			for (unsigned j = 0; j < 3; j++)
			{
				memory_arenas[i].pools[j].size_in_bytes = size_in_bytes;
				memory_arenas[i].pools[j].remaining_blocks = DEFAULT_NUM_OF_ELEMENTS;
				memory_arenas[i].pools[j].pool = (char *) malloc(memory_arenas[i].pools[j].size_in_bytes);
				memory_arenas[i].pools[j].free_slot = memory_arenas[i].pools[j].pool;

				memory_arenas[i].memory_block_size = sz;
				memory_arenas[i].pools_in_use++;

				MemBlock* head = (MemBlock*)(memory_arenas[i].pools[j].pool);
				for (unsigned k = 0; k < DEFAULT_NUM_OF_ELEMENTS ; k ++)
				{
					head->next = (MemBlock*)((char *)head + sz);
					head = (MemBlock*)head->next;
				}

				LOG_DEBUG_T(__func__, "Arena [" << i << "].pools[" << j << "] has size of : " << size_in_bytes
					<< " bytes with " << DEFAULT_NUM_OF_ELEMENTS << " blocks of " << sz << " bytes. Starting address : "
					<< (uintptr_t)memory_arenas[i].pools[j].pool << " free slot : " << (uintptr_t)memory_arenas[i].pools[j].free_slot
					<< " and end address : " << (uintptr_t) (memory_arenas[i].pools[j].pool + size_in_bytes) << " .");
			}
		}

		LOG_INFO_T(__func__, "Done for arenas [" << begin << " to " << end << ") with full size of : "
			<< size_in_bytes / 1000 << " kb / "
			<< size_in_bytes << " bytes and blocks of size : "
			<< sz);
	}

	void cleanUp();
	void removeSharedMemory();

	SharedMem shared_memory_region;
	MemArena memory_arenas[5];
};

template <typename T>
static T* allocate()
{
	return (T*) MemoryManager::getInstance().allocate<T>();
}

template <typename T>
static void deallocate(T* ptr)
{
	MemoryManager::getInstance().deallocate<T>(ptr);
}

#endif  // MEMORY_MANAGER_H_