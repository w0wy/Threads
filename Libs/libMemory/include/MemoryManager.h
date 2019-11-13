#ifndef MEMORY_MANAGER_H_
#define MEMORY_MANAGER_H_

#define SHARED_MEMORY "shared_memory"
#define DEFAULT_NUM_OF_ELEMENTS 10

#ifndef ARENAS
#define ARENAS 5
#endif 

#ifndef POOLS_PER_ARENA
#define POOLS_PER_ARENA 3
#endif

#include <sys/types.h>
#include <atomic>
#include <utility>

#include "Logger.h"

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

struct SharedMem
{
	boost::interprocess::mapped_region mapped_region;
	char* last_known_addr;
};

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
		for (unsigned i = 0; i < POOLS_PER_ARENA; i++)
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
		for (unsigned i = 0; i < POOLS_PER_ARENA; i++)
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

class MemoryManager
{
public:
	static MemoryManager& getInstance();

	template<typename T>
	inline T* allocate()
	{
		for(unsigned i = 0; i < ARENAS; i++)
		{
			if (memory_arenas[i].memory_block_size >= sizeof(T))
			{
				T* p_to_return = (T*) memory_arenas[i].allocate(sizeof(T));
				if (p_to_return != nullptr)
					return p_to_return;
			}
		}

		LOG_INFO_T(__func__, "Could not allocate ptr of type " << typeid(T).name()
			<< " and size " << sizeof(T));
		return nullptr;
	}

	template <typename T>
	inline void deallocate(T* ptrToDelete)
	{
		for (unsigned i = 0; i < ARENAS; i++)
		{
			if (memory_arenas[i].memory_block_size >= sizeof(T) &&
				memory_arenas[i].deallocate(ptrToDelete))
			{
				return;
			}
		}

		LOG_INFO_T(__func__, "Could not deallocate ptr 0x" << (uintptr_t)ptrToDelete
			<< "of type " << typeid(T).name());
	}
private:
	MemoryManager();
	virtual ~MemoryManager();
	MemoryManager(const MemoryManager&) = delete;
	MemoryManager& operator=(const MemoryManager&) = delete;

	void expandArenas(uint32_t, uint32_t, size_t);
	void expandSharedMemory();
	void initSharedMemory();

	void cleanUp();
	void removeSharedMemory();

	SharedMem shared_memory_region;
	MemArena memory_arenas[ARENAS];
};

template <typename T>
static inline T* allocate()
{
	return (T*) MemoryManager::getInstance().allocate<T>();
}

template <typename T>
static inline void deallocate(T* ptr)
{
	MemoryManager::getInstance().deallocate<T>(ptr);
}

#endif  // MEMORY_MANAGER_H_