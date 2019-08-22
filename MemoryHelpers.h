//
// Created by w0wy on 8/22/2019.
//

#ifndef THREADS_MEMORYHELPERS_H
#define THREADS_MEMORYHELPERS_H

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

enum RegionAccess
{
    read_access,
    read_write_access,
};

struct shm_remover
{
    shm_remover() {
        boost::interprocess::shared_memory_object::remove("shared_memory");}
    ~shm_remover(){
        boost::interprocess::shared_memory_object::remove("shared_memory");}
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
boost::interprocess::mapped_region getMemoryRegion(const std::string& memName, RegionAccess accesType)
{

}

boost::interprocess::mapped_region setMemoryRegion(const std::string& memName, RegionAccess accessType/*, uint32_t size*/)
{
    boost::interprocess::shared_memory_object shared_memory(
            boost::interprocess::create_only,
            memName.c_str(),
            getAccessMode(accessType));
    shared_memory.truncate(64 * 1024);

    boost::interprocess::mapped_region region(shared_memory, boost::interprocess::read_write);
    return region;
}


#endif //THREADS_MEMORYHELPERS_H
