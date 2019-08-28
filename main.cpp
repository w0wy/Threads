#include <iostream>
#include <thread>
#include <mutex>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <atomic>

#include "SyncUtils.h"
#include "Logger.h"

std::mutex mutex;
std::atomic<bool> scopedFlag;
std::atomic<bool> memoryReady;

void shared_cout(std::string msg, int id)
{
    mutex.lock();
    std::cout << msg << id << "\n";
    mutex.unlock();
}
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void normalThread()
{
    struct shm_remove
    {
        shm_remove() {
            shared_cout("shm_remove", 1);
            boost::interprocess::shared_memory_object::remove("shared_memory");}
        ~shm_remove(){
            shared_cout("~shm_remove", 0);
            boost::interprocess::shared_memory_object::remove("shared_memory");}
    } remover;

    shared_cout("in try ", 1);
    mutex.lock();
    boost::interprocess::shared_memory_object shm(
            boost::interprocess::create_only,
            "shared_memory",
            boost::interprocess::read_write);
    shm.truncate(64 * 1024);

    boost::interprocess::mapped_region region(shm, boost::interprocess::read_write);

    std::string someString("fun");
    const uint32_t *ptr = reinterpret_cast<const uint32_t*>(someString.c_str());
    std::cout <<"fucking ptr of 1 \n\n";
    std::cout << ptr[0] << std::endl;

    const char* msgs = reinterpret_cast<const char *>(ptr);
    for (int i = 0; i <= 2; i++) {
        std::cout << "reinterpret - " << *msgs++ << "\n";
    }

    std::memcpy(region.get_address(), ptr, sizeof(ptr));
    std::cout << sizeof(ptr);

    std::cout << "fucking shit : \n";
    char *memstr = reinterpret_cast<char*>(region.get_address());
    for (int i=0;i<= 2; i++)
        std::cout << *memstr++;

    memoryReady = true;
    mutex.unlock();
    shared_cout("at least here", 0);
    while(true)
    {
        mutex.lock();
        while(!scopedFlag) {
            std::cout << "no scoped flag - in mutex\n";
            scopedFlag = true;
            sleep(1);
        }
        mutex.unlock();

        mutex.lock();
        std::cout << "scoped flag - in mutex\n ";
        sleep(1);
        mutex.unlock();

        logging::Logger::getLogger()->log(std::string("scoped log"));
    }
}
#pragma clang diagnostic pop

void *posixThread(void *)
{
    for (auto i=-10; i < 0; i++)
        shared_cout("Called from posixThread!", i);

    while(!memoryReady)
        sleep(1);

    mutex.lock();
    boost::interprocess::shared_memory_object shm(
            boost::interprocess::open_only,
            "shared_memory",
            boost::interprocess::read_write);

    boost::interprocess::mapped_region region(shm, boost::interprocess::read_write);
    mutex.unlock();
    //void *mem = region.get_address();
    char *memstr = reinterpret_cast<char*>(region.get_address());
    for (int i=0;i<= 2; i++)
        std::cout << *memstr++;

    if (*memstr == 0)
        std::cout << "mem is zero\n";

}

void *secondPosixThread(void *)
{
    logging::Logger::getLogger()->log(std::string("shit log232131"));

    std::lock_guard<std::mutex> guard(mutex);
    for (auto i=0; i<=10;i++)
    {
        std::cout << "New damn posix thread : " << i << "\n";
    }


    logging::Logger::getLogger()->log(std::string("logging stuff again"));
}

int main() {

    std::thread t1(normalThread);

    //pthread_t t2;
    //pthread_create(&t2, NULL, posixThread, NULL);

    pthread_t t3;
    pthread_create(&t3, NULL, secondPosixThread, NULL);

    for (auto i=1; i <= 10; i++)
        shared_cout("Main thread!", i);

    pthread_join(t3, NULL);
    //pthread_join(t2, NULL);
    t1.join();
    return 0;
}