//
// Created by w0wy on 8/22/2019.
//

#ifndef THREADS_ASERVICE_H
#define THREADS_ASERVICE_H

#include "../UtilsLib/Logger.h"

namespace svc
{

class AService
{
public:
    AService()
    {
        static smartlog::Logger* logger_ = smartlog::Logger::getLogger();
        std::string fullTag = typeid(this).name();
//        FIXME not working as it should
//        fullTag += "+";
//        fullTag += std::to_string((int)getpid());
        logger_->setTag(fullTag);

        logger_->print("Service base initialized");
    }
    virtual ~AService()= default;

    virtual void run(char * argv[]) = 0;
};

}  // namespace svc

#endif //THREADS_ASERVICE_H
