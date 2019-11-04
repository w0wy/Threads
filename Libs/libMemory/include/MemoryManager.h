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

namespace shmm
{

class MemoryManager
{
public:
	MemoryManager();
	virtual ~MemoryManager();
	// virtual void* allocate(size_t){ return nullptr;};
	// virtual void free(void*){};
private:
	void initSharedMemory();
	void expandPoolSize();
	void cleanUp();
	void removeSharedMemory();
};

}  // namespace memhelp

#endif  // MEMORY_MANAGER_H_