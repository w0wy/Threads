#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "Message.h"
#include "MemoryManager.h"

struct MessageQueue
{
	static size_t SIZE()
	{
		return sizeof(MessageQueue); // + (DEFAULT_NUM_OF_ELEMENTS * Message::SIZE());
	}

    uint32_t        messages_count;
    Message*        front_msg;
    Message*        rear_msg;
    MessageQueue* 	next;
    size_t			size_of_data;
    Message*		data;

    void push(const Message*);
    Message * pop();

 //    void* operator new(size_t size)
	// {
 //    	LOG_INFO_T(__func__, "Size allocated is : " << size);
 //    	void * p = ::new MessageQueue();
 //    	return p;
	// }
};

#endif  // MESSAGE_QUEUE_H