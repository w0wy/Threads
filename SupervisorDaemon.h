//
// Created by w0wy on 7/28/2019.
//

#ifndef THREADS_SUPERVISORDAEMON_H
#define THREADS_SUPERVISORDAEMON_H

#include "UtilsLib/include/Logger.h"

namespace sprvs
{

class SupervisorDaemon {
public:
    SupervisorDaemon() = default;
    ~SupervisorDaemon() = default;

    void operator()(char * argv[]);
};

}

#endif //THREADS_SUPERVISORDAEMON_H
