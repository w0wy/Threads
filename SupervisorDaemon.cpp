//
// Created by w0wy on 7/28/2019.
//

#include <iostream>
#include <unistd.h>
#include <sys/stat.h>

#include "SupervisorDaemon.h"
#include "Services/Service1.h"
#include "MemoryHelpers.h"

#include <stdlib.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

Logger* SupervisorDaemon::logger_ = Logger::getLogger();

SupervisorDaemon::SupervisorDaemon()
{
    std::string fullTag = typeid(this).name();
//    FIXME not working as it should
//    fullTag += "+";
//    fullTag += std::to_string((int)getpid());
    logger_->setTag(fullTag);
}

void SupervisorDaemon::operator()()
{
    logger_->print("Daemon started. Will allocate shared memory!");
    logger_->print("Preparing remover.");
    shm_remover memRemover;
    logger_->print("Remover all set.");

    logger_->print("Preparing shared memory object and mapped region.");
    setMemoryRegion("shared_memory", RegionAccess::read_write_access);
    logger_->print("Shared memory and mapped region all set.");

    logger_->print("Will open all child processes.");

    // just to test it out
    // to do more modular
    // eg: void forkProcess(const string name) or
    // void forkProcess(const EProcessType type)
    pid_t pid, sid;

    pid = fork();

    if (pid < 0) { exit(EXIT_FAILURE); }

    if (pid == 0)
    {
        sid = setsid();
        if (sid < 0) { exit(EXIT_FAILURE); }

        Service1 service;
        service.run();
    }

    while(true)
    {
        sleep(1);
    }
}
#pragma clang diagnostic pop

int main(int argc, char * argv[])
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
