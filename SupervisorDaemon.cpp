//
// Created by w0wy on 7/28/2019.
//

#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#include "SupervisorDaemon.h"
#include "Services/Service1.h"
#include "Libs/ServicesLib/ServiceRunner.h"
#include "Libs/UtilsLib/MemoryHelpers.h"

#include <stdlib.h>

namespace sprvs
{

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

smartlog::Logger* SupervisorDaemon::logger_;

SupervisorDaemon::SupervisorDaemon()
{
    logger_ = smartlog::Logger::getLogger();
    logger_->setFullTag("sprvs::SupervisorDaemon", (int)getpid());
}

void SupervisorDaemon::operator()(char * argv[])
{
    int argv0size = strlen(argv[0]);
    strncpy(argv[0],"SupervisorDaemon",argv0size);

    logger_->print("Daemon started. Will allocate shared memory!");

    memhelp::shm_remover memRemover("shared_memory", logger_);
    memhelp::setMemoryRegion("shared_memory", memhelp::RegionAccess::read_write_access, logger_);

    logger_->print("Will start all child processes.");

    svc::startServices();

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

        svc::Service1 service;
        service.run(argv);
    }

    while(true)
    {
        sleep(1);
    }
}
#pragma clang diagnostic pop

}  // namespace supervision

int main(int argc, char * argv[])
{

    svc::startServices();

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
    sprvs::SupervisorDaemon daemon;

    // Starting daemon
    daemon(argv);

    return 0;
}
