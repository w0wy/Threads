#ifndef MEMORY_MANAGER_H_
#define MEMORY_MANAGER_H_

#define SHARED_MEMORY "shared_memory"
#define MAX_NUM_OF_QUEUES 10
#define MAX_NUM_OF_MSGS 15
#define MAX_SIZE_OF_MSG_DATA 255

#include <sys/types.h>
#include <atomic>

#include "Logger.h"
#include "MessageQueue.h"

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

namespace shmm
{

struct MemPool
{
	char * pool;
	size_t size_in_bytes;
}

class MemoryManager
{
public:
	static MemoryManager& getInstance();

	// MemoryManager();
	// virtual ~MemoryManager();
	virtual void* allocate(size_t);
	virtual void freePtr(void*);
private:
	MemoryManager();
	virtual ~MemoryManager();
	MemoryManager(const MemoryManager&) = delete;
	MemoryManager& operator=(const MemoryManager&) = delete;

	void initSharedMemory();
	void expandPoolSize();
	void cleanUp();
	void removeSharedMemory();

	boost::interprocess::mapped_region mapped_region;
	//char * internal_que_pool;
	//MessageQueue* free_que_slot;
};

}  // namespace memhelp

#endif  // MEMORY_MANAGER_H_