//
// Created by w0wy on 8/22/2019.
//

#ifndef THREADS_MEMORYHELPERS_H
#define THREADS_MEMORYHELPERS_H

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include "Logger.h"

namespace memhelp
{

enum RegionAccess
{
    read_access,
    read_write_access,
};

struct shm_remover
{
    // TODO : add logger here to notify when memory has been created
    explicit shm_remover(const std::string& memName, smartlog::Logger* logger):
        memName_(memName)
    {
        logger->print("Preparing remover.");

        boost::interprocess::shared_memory_object::remove(memName_.c_str());

        logger->print("Remover all set.");
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

// TODO add impl
boost::interprocess::mapped_region getMemoryRegion(const std::string& memName, RegionAccess accesType, smartlog::Logger* logger)
{
    logger->print("Opening shared memory object : " + memName + " and accessing mapped_region");
    
    boost::interprocess::shared_memory_object shm(
            boost::interprocess::open_only,
            memName.c_str(),
            boost::interprocess::read_write);

    boost::interprocess::mapped_region region(shm, boost::interprocess::read_write);

    logger->print("Shared memory open successful. Retrieved mapped region.");
    return region;
}

boost::interprocess::mapped_region setMemoryRegion(const std::string& memName, RegionAccess accessType, smartlog::Logger* logger/*, uint32_t size*/)
{
    logger->print("Creating shared memory object : " + memName + " and mapped region.");

    boost::interprocess::shared_memory_object shared_memory(
            boost::interprocess::create_only,
            memName.c_str(),
            getAccessMode(accessType));
    shared_memory.truncate(64 * 1024);

    boost::interprocess::mapped_region region(shared_memory, boost::interprocess::read_write);

    logger->print("Shared memory and mapped region created.");
    return region;
}

}  // namespace memhelp

#endif //THREADS_MEMORYHELPERS_H
