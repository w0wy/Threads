#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "Message.h"
#include "MemoryManager.h"

struct MessageQueue
{
	static size_t SIZE()
	{
		return sizeof(MessageQueue) + (DEFAULT_NUM_OF_ELEMENTS * Message::SIZE());
	}

    uint32_t        messages_count;
    Message*        front_msg;
    Message*        rear_msg;
    MessageQueue* 	next;
    size_t			size_of_data;
    Message*		data;

    void push(const Message*);
    Message * pop();

    void* allocate()
    {
    	shmm::MemoryManager::getInstance().allocate<MessageQueue>();
    }

    void deallocate(MessageQueue* ptr)
    {
    	shmm::MemoryManager::getInstance().deallocate<MessageQueue>((void*)ptr);
    }
};

#endif  // MESSAGE_QUEUE_H