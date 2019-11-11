#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <stddef.h>
#include <cstdint>

#define MAX_SIZE_OF_MSG_DATA 255

struct Message
{
	static size_t SIZE()
	{
		// TODO create arena with pools for primitive types
		// => no need to keep MAX_SIZE_OF_MSG_DATA
		return sizeof(Message) + MAX_SIZE_OF_MSG_DATA;
		// TODO check this -> if MAX_SIZE removed => segfault at MemoryManager.h:57
	}

	size_t      size_of_data;
    Message*    next;
	char*   	data;
};

#endif  // MESSAGE_H_