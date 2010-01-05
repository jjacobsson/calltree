/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef BTREE_MEMORY_H_INCLUDED
#define BTREE_MEMORY_H_INCLUDED

typedef unsigned int mem_size_t;
typedef void* (*AllocateMemoryFunc)( const mem_size_t size );
typedef void (*FreeMemoryFunc)( void* object_pointer );

#endif
