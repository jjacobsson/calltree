#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H


#if defined(WIN32)

#include <stddef.h>

typedef unsigned __int8 uint8;
typedef char int8;
typedef unsigned __int16 uint16;
typedef signed __int16 int16;
typedef unsigned __int32 uint32;
typedef signed __int32 int32;
typedef unsigned __int64 uint64;
typedef signed __int64 int64;

#elif defined(LINUX)

#include <stdint.h>
#include <unistd.h>

typedef uint8_t uint8;
typedef char int8;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint32_t uint32;
typedef int32_t int32;
typedef int64_t int64;
typedef uint64_t uint64;

#else
#error "Platform not supported!"
#endif

#endif // COMMON_TYPES_H
