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

#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H


#if defined(MSVC)

#include <stddef.h>

typedef unsigned __int8 uint8;
typedef char int8;
typedef unsigned __int16 uint16;
typedef signed __int16 int16;
typedef unsigned __int32 uint32;
typedef signed __int32 int32;
typedef unsigned __int64 uint64;
typedef signed __int64 int64;

#elif defined(GCC)

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
#error "Compiler not supported!"
#endif

#endif // COMMON_TYPES_H
