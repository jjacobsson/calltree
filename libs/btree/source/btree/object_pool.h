/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef OBJECTPOOL_H_
#define OBJECTPOOL_H_

#include <btree/btree_mem.h>

struct ObjectPoolSetup
{
  mem_size_t            m_TypeSize;     // How big is each object
  int                   m_BlockSize;    // How many objects should be allocated in each block
  AllocateMemoryFunc    m_Alloc;        // The function used to allocate memory
  FreeMemoryFunc        m_Free;         // The function used to free memory
};

struct ObjectPool;

ObjectPool* CreateObjectPool( ObjectPoolSetup* object_pool_setup );

void DestroyObjectPool( ObjectPool* object_pool );

void* AllocateObject( ObjectPool* object_pool );

void FreeObject( ObjectPool* object_pool, void* object );

#endif /*OBJECTPOOL_H_*/
