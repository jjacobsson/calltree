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

#ifndef OBJECTPOOL_H_
#define OBJECTPOOL_H_

#include <btree/btree_data.h>

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
