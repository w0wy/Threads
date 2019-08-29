//
// Created by w0wy on 7/28/2019.
//

#ifndef THREADS_SUPERVISORDAEMON_H
#define THREADS_SUPERVISORDAEMON_H

#include "Utils/Logger.h"

namespace sprvs
{

class SupervisorDaemon {
public:
    SupervisorDaemon();
    ~SupervisorDaemon() = default;

    void operator()(char * argv[]);

    static smartlog::Logger* logger_;
};

}

#endif //THREADS_SUPERVISORDAEMON_H
