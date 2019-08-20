//
// Created by w0wy on 7/28/2019.
//

#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <thread>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "SupervisorDaemon.h"
#include "Logger.h"

static Logger* logger_ = Logger::getLogger();

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void SupervisorDaemon::operator()()
{
    logger_->setTag(typeid(this).name());

    logger_->print("Daemon started. Will allocate shared memory!");
    logger_->print("Preparing remover.");
    struct shm_remove
    {
        shm_remove() {
            boost::interprocess::shared_memory_object::remove("shared_memory");}
        ~shm_remove(){
            boost::interprocess::shared_memory_object::remove("shared_memory");}
    } remover;
    logger_->print("Remover all set.");

    logger_->print("Preparing shared memory object and mapped region.");
    boost::interprocess::shared_memory_object shared_memory(
            boost::interprocess::create_only,
            "shared_memory",
            boost::interprocess::read_write);
    shared_memory.truncate(64 * 1024);

    boost::interprocess::mapped_region region(shared_memory, boost::interprocess::read_write);
    logger_->print("Shared memory and mapped region all set.");

    logger_->print("Will open all child processes.");
    //pid_t

    while(true)
    {
        sleep(1);
    }
}
#pragma clang diagnostic pop

int main()
{
    pid_t pid, sid;

    // Fork parent process
    pid = fork();

    if (pid < 0) { exit(EXIT_FAILURE); }

    // Close the parent process
    if (pid > 0) { exit(EXIT_SUCCESS); }

    // Change file mask
    umask(0);

    // Create new signature id for child process
    sid = setsid();

    if (sid < 0) { exit(EXIT_FAILURE); }

    // Change directory
    // If directory not found => exit with failure
    // if ((chdir("/")) < 0) { exit(EXIT_FAILURE); }

    // Close standard file descriptors
    // Daemon process should have all Input and Output closed
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Creating daemon
    SupervisorDaemon daemon;

    // Starting daemon
    daemon();

    return 0;
}
