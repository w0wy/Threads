//
// Created by w0wy on 7/28/2019.
//

#ifndef THREADS_SUPERVISORDAEMON_H
#define THREADS_SUPERVISORDAEMON_H

#include "Logger.h"
#include <vector>
#include <map>
#include <stdlib.h>

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
