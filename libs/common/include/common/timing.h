#ifndef COMMON_TIMING_H_
#define COMMON_TIMING_H_

#include <common/types.h>

inline uint64 get_cpu_counter();
inline uint64 get_cpu_frequency();

#if defined(WIN32)

#include <windows.h>

inline uint64 get_cpu_frequency()
{
    LARGE_INTEGER t;
    QueryPerformanceFrequency(&t);
    return (uint64)t.QuadPart;
}

inline uint64 get_cpu_counter()
{
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return (uint64)t.QuadPart;
}

#elif defined(LINUX)

#include <sys/time.h>
#include <time.h>

inline uint64 get_cpu_frequency()
{
    return 1000000ul;
}

inline uint64 get_cpu_counter()
{
    timeval time_struct;
    gettimeofday(&time_struct, NULL);
    return (uint64)time_struct.tv_sec*1000000ul + (uint64)time_struct.tv_usec;
}

#else
#error Unknown platform!
#endif

#endif // COMMON_TIMING_H_
