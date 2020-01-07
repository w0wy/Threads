#ifndef MEMORY_MANAGER_H_
#define MEMORY_MANAGER_H_

#define SHARED_MEMORY "shared_memory"
#define DEFAULT_NUM_OF_ELEMENTS 10

#ifndef ARENAS
#define ARENAS 5
#endif

#ifndef POOLS
#define POOLS 3
#endif

#ifndef BEGIN_SYM
#define BEGIN_SYM '#'
#endif

#ifndef POOL_HEADER
#define POOL_HEADER 2 * sizeof(char *) + 1
#endif

#include <sys/types.h>
#include <utility>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include "exceptions/BadAllocException.h"

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
	uint32_t remaining_blocks;
	char* pool;
	char* free_slot;
};

struct PoolHeader
{
	char * arena;
	char * pool;
};

struct MemArena
{
	uint32_t pools_in_use;
	size_t memory_block_size;
	uint32_t free_slot_pool;
	MemPool pools[POOLS];

	inline char* allocate(size_t sz)
	{
		if (free_slot_pool >= pools_in_use or
			pools[free_slot_pool].remaining_blocks == 0)
		{
			return nullptr;
		}

		char * toReturn = pools[free_slot_pool].free_slot;
		pools[free_slot_pool].remaining_blocks--;
		if (pools[free_slot_pool].remaining_blocks == 0)
		{
			pools[free_slot_pool].free_slot = nullptr;
		}
		else
		{
			pools[free_slot_pool].free_slot = (char *) ((MemBlock*)pools[free_slot_pool].free_slot)->next;
		}

		set_free_slot_pool();
		return toReturn;
	}

	inline void deallocate(void * ptrToDelete, MemPool* pool)
	{
		char * last_free_slot = pool->free_slot;
		pool->free_slot = (char*) ptrToDelete;

		if (last_free_slot != nullptr)
			((MemBlock*)(pool->free_slot))->next = (MemBlock*)last_free_slot;
		else
			((MemBlock*)(pool->free_slot))->next = nullptr;

		pool->remaining_blocks++;
		if (&pool[0] == pool)
			free_slot_pool = 0;
		else
			if (&pool[1] == pool)
				free_slot_pool = 1;
			else
				if(&pool[2] == pool)
					free_slot_pool = 2;

		//set_free_slot_pool();
	}

private:
	inline void set_free_slot_pool()
	{
		bool done = false;
		for (unsigned cnt = 0; cnt < POOLS; cnt++)
		{
			if (pools[cnt].remaining_blocks > 0)
			{
				done = true;
				free_slot_pool = cnt;
				return;
			}
		}

		free_slot_pool = 4;
	}
};

class MemoryManager
{
public:
	static inline MemoryManager& getInstance()
	{
		static MemoryManager instance;
		return instance;
	}

	template<typename T>
	inline T* allocate()
	{
		for(unsigned i = 0; i < ARENAS; i++)
		{
			if (memory_arenas[i].memory_block_size >= sizeof(T)
				and memory_arenas[i].free_slot_pool < 4)
			{
				T* p_to_return = (T*) memory_arenas[i].allocate(sizeof(T));
				if (p_to_return != nullptr)
					return p_to_return;
			}
		}
		throw BadAllocException("allocate", typeid(T).name(), (uintptr_t)0);
	}

	template <typename T>
	inline void deallocate(T* ptrToDelete)
	{
		char * indexer = (char *) ptrToDelete;
		char b = '#';
		while (memcmp(indexer, &b, 1) != 0)
		{
			--indexer;
		}
		PoolHeader* header = (PoolHeader*) (++indexer);

		MemArena * dealloc_arena = (MemArena*)header->arena;
		MemPool * dealloc_pool = (MemPool*)header->pool;

		if (dealloc_arena == nullptr ||
			dealloc_pool == nullptr)
			throw BadAllocException("deallocate", typeid(T).name(), (uintptr_t)ptrToDelete);
		
		dealloc_arena->deallocate(ptrToDelete, dealloc_pool);
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
	return MemoryManager::getInstance().allocate<T>();
}

template <typename T>
static inline void deallocate(T* ptr)
{
	MemoryManager::getInstance().deallocate<T>(ptr);
}

#endif  // MEMORY_MANAGER_H_

