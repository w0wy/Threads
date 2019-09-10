//
// Created by w0wy on 8/22/2019.
//

#ifndef THREADS_MEMORYHELPERS_H
#define THREADS_MEMORYHELPERS_H

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include "Utilities.h"

#include "Logger.h"

#define F_PROCESS_UID 0x03E9 // 1001

namespace memhelp
{

static boost::interprocess::mapped_region region_;

enum RegionAccess
{
    read_access,
    read_write_access,
};

struct Message
{
    uint32_t * data;
    size_t size_of_data;
};

struct MessageQueue
{
    MessageQueue():
    msgs_(new Message[10]),
    capacity_(10),
    front_(0),
    rear_(0),
    count_(0)
    {};
    ~MessageQueue()
    {
        delete[] msgs_;
    };

private:
    Message * msgs_;
    int capacity_;
    int front_;
    int rear_; 
    int count_;
};

static constexpr size_t message_queue_size_ = sizeof(MessageQueue);

struct shm_remover
{
    explicit shm_remover(const std::string& memName):
        memName_(memName)
    {
        boost::interprocess::shared_memory_object::remove(memName_.c_str());
        LOG_DEBUG("Previously allocated memory object [" << memName << "] has been removed.");
    }
    ~shm_remover()
    {
        boost::interprocess::shared_memory_object::remove(memName_.c_str());
        LOG_DEBUG("Allocated memory object [" << memName_ << "] has been removed.");
    }

    std::string memName_;
};


namespace {
    boost::interprocess::mode_t getAccessMode(RegionAccess access) {
        switch(access)
        {
            case RegionAccess::read_write_access:
                return boost::interprocess::read_write;
            case RegionAccess::read_access:
            default:
                return boost::interprocess::read_only;
        }
    }
}

void setSharedMemory(const std::string& memName, RegionAccess accessType)
{
    boost::interprocess::shared_memory_object shared_memory(
            boost::interprocess::create_only,
            memName.c_str(),
            getAccessMode(accessType));
    shared_memory.truncate(64 * 1024);
    region_ = boost::interprocess::mapped_region(shared_memory, getAccessMode(accessType));

    LOG_DEBUG("Shared memory object [" << memName << "] has been created and mapped_region has been set.");
}

void registerCommunication(int uid)
{
    MessageQueue msgQue;
    std::memcpy(static_cast<char*>(region_.get_address()) + (uid % 10 * message_queue_size_), 
        &msgQue, message_queue_size_);

    LOG_DEBUG("Process [" << uid << "] registered successfuly for communication.")
}

void sendTo(Message msg, int uid)
{
    UNUSED(msg);
    UNUSED(uid);
}

}  // namespace memhelp

#endif //THREADS_MEMORYHELPERS_H
