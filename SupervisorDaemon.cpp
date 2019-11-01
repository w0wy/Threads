//
// Created by w0wy on 7/28/2019.
//

#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#include "SupervisorDaemon.h"
#include "Services/Service1.h"

#include "ServiceRunner.h"
#include "Utilities.h"

#include <stdlib.h>
#include <cstring>

namespace sprvs
{

void SupervisorDaemon::operator()(char * argv[])
{
    int argv0size = strlen(argv[0]);
    strncpy(argv[0],"SupervisorDaemon",argv0size);

    LOG_INFO("Daemon started. Will allocate shared memory for processes...");

    // MemoryManager setup

    LOG_INFO("Shared memory allocated. Will start processes...");

    //while(true)
    //{
    //    sleep(1);
    //}
}

}  // namespace sprvs

int main(int argc, char * argv[])
{
    UNUSED(argc);
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
