#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "Message.h"

struct MessageQueue
{
    uint32_t        messages_count;
    Message*        front_msg;
    Message*        rear_msg;
    size_t			size_of_data;
    Message*		data;

    void push(const Message*);
    Message * pop();
};

#endif  // MESSAGE_QUEUE_H