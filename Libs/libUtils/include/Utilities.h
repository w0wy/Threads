#ifndef THREADS_UTILITIES_H
#define THREADS_UTILITIES_H

#define UNUSED(expr) do { (void)(expr); } while (0)

#include <stdlib.h>
#include <execinfo.h>

namespace tracing
{

std::string get_call_trace() {
    char **strings;
    size_t size;
    enum Constexpr { MAX_SIZE = 1024 };
    void *array[MAX_SIZE];
    size = backtrace(array, MAX_SIZE);
    strings = backtrace_symbols(array, size);

    std::string call_trace;
    for (size_t i=0; i < size; i++)
    {
        call_trace += strings[i];
        call_trace += "\n";
    }
    free(strings);

    return call_trace;
}

}

#endif  // THREADS_UTILITIES_H