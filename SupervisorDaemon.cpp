//
// Created by w0wy on 7/28/2019.
//

#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <thread>

#include "Logger.h"
#include "SyncUtils.h"

#include "SupervisorDaemon.h"

std::mutex localMutex_;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void SupervisorDaemon::operator()(int x)
{
    logging::Logger::getLogger()->log(std::string("test the damned logger"));
    //syncUtils::shared_cout(localMutex_, "SupervisorDaemon started!");

    while(true)
    {
        sleep(1);
    }
}
#pragma clang diagnostic pop


int main()
{
    syncUtils::shared_cout(localMutex_, "Got in main!");
    pid_t pid, sid;

    logging::Logger::getLogger()->log(std::string("shit log"));

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

    logging::Logger::getLogger()->log(std::string("fucking logger"));

    // Creating daemon
    SupervisorDaemon daemon;

    // Starting daemon
    daemon(1);

    return 0;
}
