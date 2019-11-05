#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "Message.h"
#include "MemoryManager.h"

struct MessageQueue
{
    uint32_t        messages_count;
    Message*        front_msg;
    Message*        rear_msg;
    Message*		messages;
    MessageQueue* next;

    void push(const Message*);
    Message * pop();
};

#endif  // MESSAGE_QUEUE_H