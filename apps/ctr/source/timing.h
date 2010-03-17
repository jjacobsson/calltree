/*******************************************************************************
 * Copyright (c) 2009-04-24 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#ifndef CTR_TIMING_H_
#define CTR_TIMING_H_

#if defined(MSVC)

#include <windows.h>

typedef unsigned __int64 uint64;
typedef signed __int64 int64;


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

#elif defined(GCC)

#include <sys/time.h>
#include <time.h>

#include <stdint.h>
#include <unistd.h>

typedef int64_t int64;
typedef uint64_t uint64;

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
#error "Compiler not supported."
#endif

#endif // CTR_TIMING_H_
