#ifndef MEMORY_MANAGER_H_
#define MEMORY_MANAGER_H_

#define SHARED_MEMORY "shared_memory"
#define DEFAULT_NUM_OF_ELEMENTS 10

#include <sys/types.h>
#include <atomic>

#include "Logger.h"

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

struct MemPool
{
	size_t size_in_bytes;
	size_t remaining_blocks;
	char* pool;
	char* free_slot;
};

// TODO maybe specialize arenas for different types :
// ARENA[0->3) for MessageQueues
// ARENA[3->5) for Message
// ARENA[6->7) for primitive types
// ETC

// maybe with sfinae 
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

				if (pools[i].remaining_blocks == 0)
				{
					pools[i].free_slot = nullptr;
					return toReturn;
				}

				if (memory_block_size > sz)
				{
					unsigned diff = memory_block_size - sz;
					pools[i].free_slot = (char *)((char*)(((T*)pools[i].free_slot)->next) + diff);
				}
				else
				{
					pools[i].free_slot = (char*) (((T*)pools[i].free_slot)->next);
					// TODO : BIG PROBLEM
					// both message queue and message might have same size but "MSGQUEUE->next might not be the same with MSG->next" 
				}

				return toReturn;
			}
		}

		return nullptr;
	}

	template <typename T>
	bool deallocate(void * ptrToDelete, size_t sz)
	{
		for (unsigned i = 0; i < 3; i++)
		{
			if ((char*)ptrToDelete >= pools[i].pool &&
				(char*)ptrToDelete <= (pools[i].pool + pools[i].size_in_bytes))
			{
				char * last_free_slot = pools[i].free_slot;
				pools[i].free_slot = (char*) ptrToDelete;
				
				if (last_free_slot != nullptr)
					((T*)pools[i].free_slot)->next = (T*)last_free_slot;
				else
					((T*)pools[i].free_slot)->next = nullptr;

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
			if (memory_arenas[i].memory_block_size >= T::SIZE())
			{
				T* p_to_return = (T*) memory_arenas[i].allocate<T>(T::SIZE());
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
			if (memory_arenas[i].memory_block_size >= T::SIZE() &&
				memory_arenas[i].deallocate<T>(ptrToDelete, T::SIZE()))
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

				memory_arenas[i].memory_block_size = T::SIZE();
				memory_arenas[i].pools_in_use++;

				T* head = (T*)(memory_arenas[i].pools[j].pool);
				for (unsigned k = 0; k < DEFAULT_NUM_OF_ELEMENTS ; k ++)
				{
					head->next = (T*)((char *)head + T::SIZE());
					head = (T*)head->next;
				}

				LOG_DEBUG_T(__func__, "Arena [" << i << "].pools[" << j << "] has size of : " << size_in_bytes
					<< " bytes with " << DEFAULT_NUM_OF_ELEMENTS << " blocks of " << T::SIZE() << " bytes. Starting address : "
					<< (uintptr_t)memory_arenas[i].pools[j].pool << " free slot : " << (uintptr_t)memory_arenas[i].pools[j].free_slot
					<< " and end address : " << (uintptr_t) (memory_arenas[i].pools[j].pool + size_in_bytes) << " .");
			}
		}

		LOG_INFO_T(__func__, "Done for arenas [" << begin << " to " << end << ") with full size of : "
			<< size_in_bytes / 1000 << " kb / "
			<< size_in_bytes << " bytes and blocks of size : "
			<< T::SIZE());
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