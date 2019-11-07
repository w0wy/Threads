#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <stddef.h>
#include <cstdint>

#include "MemoryManager.h"

#define MAX_SIZE_OF_MSG_DATA 255

struct Message
{
	static size_t SIZE()
	{
		return sizeof(Message) + MAX_SIZE_OF_MSG_DATA;
	}
	
	size_t      size_of_data;
    Message*    next;
	uint32_t*   data;
};

#endif  // MESSAGE_H_