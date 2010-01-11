/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef BTREE_HEADER_INCLUDED
#define BTREE_HEADER_INCLUDED

#include "btree_data.h"
#include "btree_func.h"
#include "btree_mem.h"

typedef struct SBehaviorTreeContext* BehaviorTreeContext;

struct BehaviorTreeContextSetup
{
  AllocateMemoryFunc m_Alloc; // The function that will be used for all memory allocations
  FreeMemoryFunc m_Free;      // The function that will be used to free all allocated memory
};

BehaviorTreeContext BehaviorTreeContextCreate( BehaviorTreeContextSetup* );

void BehaviorTreeContextDestroy( BehaviorTreeContext );

const char* RegisterString( BehaviorTreeContext, const char* );

const char* RegisterString( BehaviorTreeContext, const char*, hash_t );



#endif /*BTREE_HEADER_INCLUDED*/
