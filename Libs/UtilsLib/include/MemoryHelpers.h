//
// Created by w0wy on 8/22/2019.
//

#ifndef THREADS_MEMORYHELPERS_H
#define THREADS_MEMORYHELPERS_H

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include "Logger.h"

#define F_PROCESS_UID 0x3E9 // 1001

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
    int capacity_;

private:
    Message * msgs_;
    int front_;
    int rear_; 
    int count_;
};

static constexpr size_t message_queue_size_ = sizeof(MessageQueue);

struct shm_remover
{
    // TODO : add logger here to notify when memory has been created
    explicit shm_remover(const std::string& memName)://, smartlog::Logger* logger):
        memName_(memName)
    {
        //logger->print("Preparing remover.");
        boost::interprocess::shared_memory_object::remove(memName_.c_str());
        //logger->print("Remover all set.");
    }
    ~shm_remover()
    {
        boost::interprocess::shared_memory_object::remove(memName_.c_str());
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

void setSharedMemory(const std::string& memName, RegionAccess accessType)//, smartlog::Logger* logger/*, uint32_t size*/)
{
    //logger->print("Creating shared memory object [" + memName + "] and mapped region.");

    boost::interprocess::shared_memory_object shared_memory(
            boost::interprocess::create_only,
            memName.c_str(),
            getAccessMode(accessType));
    shared_memory.truncate(64 * 1024);
    region_ = boost::interprocess::mapped_region(shared_memory, getAccessMode(accessType));

    //logger->print("Shared memory object created.");
}

void registerCommunication(int uid)//, smartlog::Logger* logger)
{
    //logger->print("Registering process [" + std::to_string(uid) + "] to communcation!");

    MessageQueue msgQue;
    std::memcpy(static_cast<char*>(region_.get_address()) + (uid % 10 * message_queue_size_), 
        &msgQue, message_queue_size_);

    //logger->print("Process [" + std::to_string(uid) + "] registered successfuly!");
}

void sendTo(Message msg, int uid)
{
    //MessageQueue * msg = reinterpret_cast<MessageQueue*>(uid % 10 * message_queue_size_);
}

}  // namespace memhelp

#endif //THREADS_MEMORYHELPERS_H
