//
// Created by w0wy on 8/22/2019.
//

#ifndef THREADS_SERVICE1_H
#define THREADS_SERVICE1_H

#include "AService.h"

// TODO move impl to .cpp
class Service1 : public AService
{
public:
    Service1()
    {
        // just to test it out
        static Logger* logger_ = Logger::getLogger();
        std::string fullTag = typeid(this).name();
//        FIXME not working as it should
//        fullTag += "+";
//        fullTag += std::to_string((int)getpid());
        logger_->setTag(fullTag);

        logger_->print("Service initialized");
    };
    virtual ~Service1() = default;

    void run() {

        // just to test it out
        while(true) {
            static Logger *logger_ = Logger::getLogger();
            logger_->setTag(typeid(this).name());

            logger_->print("running");
            sleep(10);
        }
    }
};

#endif //THREADS_SERVICE1_H
