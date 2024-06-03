#pragma once
#include <iostream>

class NullStream {
public:
    template<typename T>
    NullStream& operator<<(const T&) { return *this; }
};

static NullStream null_stream;

#if defined(DEBUG_PRINT) && (DEBUG_PRINT == 1)
    #define DEBUG_COUT std::cout
#else
    #define DEBUG_COUT null_stream
#endif
