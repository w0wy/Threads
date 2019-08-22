//
// Created by w0wy on 8/22/2019.
//

#ifndef THREADS_ASERVICE_H
#define THREADS_ASERVICE_H

#include "../Logger.h"

class AService
{
public:
    AService()
    {
        static Logger* logger_ = Logger::getLogger();
        logger_->setTag(typeid(this).name());

        logger_->print("Service base initialized");
    }
    virtual ~AService()= default;

    virtual void run() = 0;
};


#endif //THREADS_ASERVICE_H
