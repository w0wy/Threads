//
// Created by w0wy on 7/28/2019.
//

#ifndef THREADS_SYNCUTILS_H
#define THREADS_SYNCUTILS_H

#include <string>
#include <mutex>
#include <iostream>

namespace syncUtils {
    void shared_cout(std::mutex &mutex, const std::string &msg) {
        std::lock_guard<std::mutex> guard(mutex);
        std::cout << "\n" + msg + "\n";
    }
}
#endif //THREADS_SYNCUTILS_H
