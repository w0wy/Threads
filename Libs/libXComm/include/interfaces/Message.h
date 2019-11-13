#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <stddef.h>
#include <cstdint>

#define MAX_SIZE_OF_MSG_DATA 255

struct Message
{
	size_t      size_of_data;
	char*   	data;
};

#endif  // MESSAGE_H_