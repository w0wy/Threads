#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <stddef.h>
#include <cstdint>

struct Message
{
    size_t      size_of_data;
    uint32_t*   data;
    Message*    next_msg;
};

#endif  // MESSAGE_H_