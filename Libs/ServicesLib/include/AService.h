//
// Created by w0wy on 8/22/2019.
//

#ifndef THREADS_ASERVICE_H
#define THREADS_ASERVICE_H

#include "UtilsLib/include/Logger.h"

namespace svc
{

class AService
{
public:
    AService() = default;
    virtual ~AService() = default;

    virtual void run(char * argv[]) = 0;
};

}  // namespace svc

#endif //THREADS_ASERVICE_H
