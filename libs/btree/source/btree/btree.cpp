/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include <other/lookup3.h>

#include <btree/btree_data.h>
#include <btree/btree_func.h>

#include "../parser/common.h"
#include "sym_table.h"
#include "object_pool.h"
#include "string_table.h"

struct SBehaviorTreeContext
{
  StringTable               m_StringTable;
  SymbolTable               m_SymbolTable;
  BehaviorTreeContextSetup  m_Setup;
  BehaviorTree*             m_Trees;
  ObjectPool*               m_Pool;
};

union ObjectFootPrint
{
  Variable              m_Variable;
  Action                m_Action;
  Decorator             m_Decorator;
  Node                  m_Node;
  BehaviorTree          m_Tree;
  SBehaviorTreeContext  m_BTContext;
  SParserContext        m_ParserContext;
};

BehaviorTreeContext BehaviorTreeContextCreate( BehaviorTreeContextSetup* btcs )
{
  ObjectPoolSetup ops;
  ops.m_Alloc       = btcs->m_Alloc;
  ops.m_Free        = btcs->m_Free;
  ops.m_BlockSize   = 4096;
  ops.m_TypeSize    = (mem_size_t)sizeof(ObjectFootPrint);
  ObjectPool* op    = CreateObjectPool( &ops );

  SBehaviorTreeContext* btc =
      &(((ObjectFootPrint*)AllocateObject( op ))->m_BTContext);
  btc->m_Trees          = 0x0;
  btc->m_Pool           = op;
  btc->m_Setup          = *btcs;

  StringTableInit( &btc->m_StringTable, btcs->m_Alloc, btcs->m_Free );
  SymbolTableInit( &btc->m_SymbolTable, btcs->m_Alloc, btcs->m_Free );

  return btc;
}

void BehaviorTreeContextDestroy( BehaviorTreeContext btc )
{
  if( !btc )
    return;

  StringTableDestroy( &btc->m_StringTable );
  SymbolTableDestroy( &btc->m_SymbolTable );

  ObjectPool* op = btc->m_Pool;
  FreeObject( op, btc );
  DestroyObjectPool( op );
}

const char* RegisterString( BehaviorTreeContext btc, const char* str )
{
    return StringTableRegisterString( &btc->m_StringTable, str, hashlittle( str ) );
}

const char* RegisterString( BehaviorTreeContext btc, const char* str, hash_t hash )
{
    return StringTableRegisterString( &btc->m_StringTable, str, hash );
}

ParserContext ParserContextCreate( BehaviorTreeContext btc )
{
  SParserContext* pc = &(((ObjectFootPrint*)AllocateObject( btc->m_Pool ))->m_ParserContext);

  pc->m_Tree    = btc;
  pc->m_LineNo  = 0;
  pc->m_Error   = 0x0;
  pc->m_Warning = 0x0;
  pc->m_Read    = 0x0;
  pc->m_Extra   = 0x0;
  pc->m_Alloc   = btc->m_Setup.m_Alloc;
  pc->m_Free    = btc->m_Setup.m_Free;
  StringBufferInit( pc, &pc->m_Parsed );
  StringBufferInit( pc, &pc->m_Original );
  return pc;
}

void ParserContextDestroy( ParserContext pc )
{
  StringBufferDestroy( pc, &pc->m_Parsed );
  StringBufferDestroy( pc, &pc->m_Original );
  FreeObject( pc->m_Tree->m_Pool, pc );
}

void* AllocateObject( BehaviorTreeContext btc )
{
  return AllocateObject( btc->m_Pool );
}

void FreeObject( BehaviorTreeContext btc, void* object )
{
  FreeObject( btc->m_Pool, object );
}

void RegisterSymbol( BehaviorTreeContext ctx, const NamedSymbol& s )
{
  SymbolTableInsert( &ctx->m_SymbolTable, s );
}

NamedSymbol* FindSymbol( BehaviorTreeContext ctx, hash_t hash )
{
  return SymbolTableFind( &ctx->m_SymbolTable, hash );
}
