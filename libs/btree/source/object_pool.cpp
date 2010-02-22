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

#include "object_pool.h"

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

#define OP_ALLOC_MACRO( _size ) op->m_Setup.m_Allocator.m_Alloc(_size)
#define OP_FREE_MACRO( _ptr ) op->m_Setup.m_Allocator.m_Free(_ptr)

void AllocateBlock( ObjectPool* op );
void SetupBlock( ObjectPool* op, SBlock* block );

ObjectPool* CreateObjectPool( ObjectPoolSetup* ops )
{
  mem_size_t type_size =
      ops->m_TypeSize > (mem_size_t)sizeof(SObject) ? ops->m_TypeSize
        : (mem_size_t)sizeof(SObject);

  ObjectPool* op = (ObjectPool*)ops->m_Allocator.m_Alloc(
    (mem_size_t)sizeof(ObjectPool) +
    (type_size * ops->m_BlockSize)
  );
  op->m_Setup = *ops;
  op->m_FirstBlock = 0x0;
  op->m_FirstFree = 0x0;
  op->m_Setup.m_TypeSize = type_size;
  op->m_JoinedBlock = (SBlock*)(((char*)op) + sizeof( ObjectPool ));
  SetupBlock( op, op->m_JoinedBlock );

  return op;
}

void DestroyObjectPool( ObjectPool* op )
{
  SBlock* b = op->m_FirstBlock;
  while( b )
  {
    op->m_FirstBlock = b->m_Next;
    if( b != op->m_JoinedBlock )
      OP_FREE_MACRO( b );
    b = op->m_FirstBlock;
  }
  OP_FREE_MACRO( op );
}

void* AllocateObject( ObjectPool* op )
{
  //Get the first free SObject
  SObject* o( op->m_FirstFree );
  if( o )
  {
    //If we got one, just advance first free
    op->m_FirstFree = o->m_Next;
  }
  else
  {
    //There was no free SObject, so we allocate a new block and add it to the pool
    AllocateBlock( op );
    //Get the first free SObject
    o = op->m_FirstFree;
    //If we did'nt get one this time we return 0x0, failed alloc
    if( !o )
      return 0x0;
    //Otherwise advance first free
    op->m_FirstFree = o->m_Next;
  }
  //And return the SObject as a (void*)
  return (void*) (o);
}

void FreeObject( ObjectPool* op, void* ptr )
{
  //Early out on null argument
  if( !ptr )
    return;
  //Cast to SObject
  SObject* o = (SObject*) (ptr);
  //Set next pointer to first free
  o->m_Next = op->m_FirstFree;
  //Set first free to freed object
  op->m_FirstFree = o;
}

void AllocateBlock( ObjectPool* op )
{
  //Calculate the memory needed for a block
  const mem_size_t alloc_size = (mem_size_t)(sizeof(SBlock)
      + op->m_Setup.m_TypeSize * op->m_Setup.m_BlockSize);

  //Allocate memory for a new block
  SBlock* block = (SBlock*)OP_ALLOC_MACRO( alloc_size );
  //Setup the data in the block and add it to the pool
  SetupBlock( op, block );
}

void SetupBlock( ObjectPool* op, SBlock* block )
{
  //Set m_Next to current first block
  block->m_Next = op->m_FirstBlock;

  //Set first block the the freshly allocated block
  op->m_FirstBlock = block;

  //Set m_First (in the block) to the memory directly after the block...
  block->m_First = (SObject*)(((char*)block) + (mem_size_t)sizeof(SBlock));

  //Temporary to make it easier to setup the next pointers
  SObject* tmp = block->m_First;

  //Form a new free list by linking the memory chunks together.
  for( int i = 0; i < op->m_Setup.m_BlockSize - 1; ++i )
  {
    tmp->m_Next = (SObject*)(((char*)tmp) + op->m_Setup.m_TypeSize);
    tmp = tmp->m_Next;
  }

  //Set the last next pointer to the pool's current first.
  tmp->m_Next = op->m_FirstFree;

  //Set the pool's first to the block's first.
  op->m_FirstFree = block->m_First;
}

#undef OP_ALLOC_MACRO
#undef OP_FREE_MACRO

