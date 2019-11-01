#ifndef SHARED_MEMORY_MANAGER_H_
#define SHARED_MEMORY_MANAGER_H_

#define SHARED_MEMORY "shared_memory"
#define MAX_NUM_OF_QUEUES 10
#define MAX_NUM_OF_MSGS 15
#define MAX_SIZE_OF_MSG_DATA 255

#include <sys/types.h>

#include "Logger.h"

namespace shmm
{

class SharedMemoryManager
{
	void initSharedMemory();
	void expandPoolSize();
	void cleanUp();
	void removeSharedMemory();

public:
	SharedMemoryManager();
	virtual ~SharedMemoryManager();
	virtual void* allocate(size_t){ return nullptr;};
	virtual void free(void*){};
};

}  // namespace memhelp

#endif  // SHARED_MEMORY_MANAGER_H_