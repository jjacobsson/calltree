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

#ifndef OBJECTPOOL_H_INCLUDED
#define OBJECTPOOL_H_INCLUDED

#include <btree/btree_data.h>

struct ObjectPoolSetup
{
  mem_size_t m_TypeSize;     // How big is each object
  int        m_BlockSize;    // How many objects should be allocated in each block
  Allocator  m_Allocator;
};

struct SObject
{
  SObject* m_Next;
};

struct SBlock
{
  SBlock*  m_Next;
  SObject* m_First;
};

struct ObjectPool
{
  ObjectPoolSetup m_Setup;
  SBlock*         m_JoinedBlock;
  SBlock*         m_FirstBlock;
  SObject*        m_FirstFree;
};

ObjectPool* create_object_pool( ObjectPoolSetup* object_pool_setup );

void destroy( ObjectPool* object_pool );

void* allocate_object( ObjectPool* object_pool );

void free_object( ObjectPool* object_pool, void* object );

#endif /*OBJECTPOOL_H_INCLUDED*/
