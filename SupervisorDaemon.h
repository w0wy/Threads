//
// Created by w0wy on 7/28/2019.
//

#ifndef THREADS_SUPERVISORDAEMON_H
#define THREADS_SUPERVISORDAEMON_H

class SupervisorDaemon {
public:
    SupervisorDaemon() = default;
    ~SupervisorDaemon() = default;

    void operator()();
};


#endif //THREADS_SUPERVISORDAEMON_H
