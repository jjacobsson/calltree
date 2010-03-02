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

#include <other/lookup3.h>

#include <btree/btree_data.h>
#include <btree/btree_func.h>

#include "btree_internal.h"

#include <string.h> // for memset....

BehaviorTreeContext BehaviorTreeContextCreate( Allocator& allocator )
{
  ObjectPoolSetup ops;
  ops.m_Allocator = allocator;
  ops.m_BlockSize = 4096;
  ops.m_TypeSize  = (mem_size_t)sizeof(ObjectFootPrint);
  ObjectPool* op  = CreateObjectPool( &ops );

  SBehaviorTreeContext* btc =
      &(((ObjectFootPrint*)AllocateObject( op ))->m_BTContext);
  btc->m_Pool      = op;
  btc->m_Allocator = allocator;
  btc->m_Includes  = 0x0;

  StringTableInit( &btc->m_StringTable, btc->m_Allocator );
  SymbolTableInit( &btc->m_SymbolTable, btc->m_Allocator );

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

const char* BehaviorTreeContextRegisterString( BehaviorTreeContext btc, const char* str )
{
    return StringTableRegisterString( &btc->m_StringTable, str, hashlittle( str ) );
}

const char* BehaviorTreeContextRegisterString( BehaviorTreeContext btc, const char* str, hash_t hash )
{
    return StringTableRegisterString( &btc->m_StringTable, str, hash );
}

void* BehaviorTreeContextAllocateObject( BehaviorTreeContext btc )
{
  return AllocateObject( btc->m_Pool );
}

void BehaviorTreeContextFreeObject( BehaviorTreeContext btc, void* object )
{
  FreeObject( btc->m_Pool, object );
}

void BehaviorTreeContextRegisterSymbol( BehaviorTreeContext btc, const NamedSymbol& s )
{
  SymbolTableInsert( &btc->m_SymbolTable, s );
}

void BehaviorTreeContextRemoveSymbol( BehaviorTreeContext btc, hash_t hash )
{
  SymbolTableErase( &btc->m_SymbolTable, hash );
}

NamedSymbol* BehaviorTreeContextFindSymbol( BehaviorTreeContext btc, hash_t hash )
{
  return SymbolTableFind( &btc->m_SymbolTable, hash );
}

NamedSymbol* BehaviorTreeContextAccessSymbols( BehaviorTreeContext btc, int* count )
{
  *count = btc->m_SymbolTable.m_Size;
  return btc->m_SymbolTable.m_Symbols;
}

void BehaviorTreeContextAddInclude( BehaviorTreeContext btc, const Include& include )
{
  hash_t h = hashlittle( include.m_Name );
  Include* t = btc->m_Includes;
  Include* l = 0x0;
  while( t )
  {
    if( t->m_Hash == h )
      return;
    l = t;
    t = t->m_Next;
  }

  Include* i = &(((ObjectFootPrint*)AllocateObject( btc->m_Pool ))->m_Include);
  *i = include;
  i->m_Hash = h;
  i->m_Next = 0x0;
  i->m_UserData = 0x0;

  if( l )
    l->m_Next = i;
  else
    btc->m_Includes = i;
}

Include* BehaviorTreeContextCreateInclude( BehaviorTreeContext btc, const char* path_in )
{
  const char* path = BehaviorTreeContextRegisterString( btc, path_in );
  hash_t h = hashlittle( path );
  Include* t = btc->m_Includes;
  Include* l = 0x0;
  while( t )
  {
    if( t->m_Hash == h )
      return 0x0;
    l = t;
    t = t->m_Next;
  }

  Include* i = &(((ObjectFootPrint*)AllocateObject( btc->m_Pool ))->m_Include);
  i->m_LineNo = 0;
  i->m_Name = path;
  i->m_Hash = h;
  i->m_Parent = 0x0;
  i->m_Next = 0x0;
  i->m_UserData = 0x0;

  if( l )
    l->m_Next = i;
  else
    btc->m_Includes = i;
  return i;
}

Include* BehaviorTreeContextGetFirstInclude( BehaviorTreeContext btc )
{
  return btc->m_Includes;
}

void BehaviorTreeContextReleaseInclude( BehaviorTreeContext btc, Include* i )
{
  Include* t = btc->m_Includes;
  Include* p = 0x0;
  while( t )
  {
    if( t == i )
    {
      if( p )
        p->m_Next = i->m_Next;
      else
        btc->m_Includes = i->m_Next;
      break;
    }
    p = t;
    t = t->m_Next;
  }
  BehaviorTreeContextFreeObject( btc, i );
}

ParserContext ParserContextCreate( BehaviorTreeContext btc )
{
  SParserContext* pc = &(((ObjectFootPrint*)AllocateObject( btc->m_Pool ))->m_ParserContext);

  pc->m_Tree        = btc;
  pc->m_LineNo      = 0;
  pc->m_Extra       = 0x0;
  pc->m_Current     = 0x0;
  pc->m_Allocator   = btc->m_Allocator;
  StringBufferInit( pc->m_Allocator, &pc->m_Parsed );
  StringBufferInit( pc->m_Allocator, &pc->m_Original );
  memset( &pc->m_Funcs, 0, sizeof(ParserContextFunctions) );
  return pc;
}

void ParserContextDestroy( ParserContext pc )
{
  StringBufferDestroy( &pc->m_Parsed );
  StringBufferDestroy( &pc->m_Original );
  FreeObject( pc->m_Tree->m_Pool, pc );
}

SaverContext SaverContextCreate( BehaviorTreeContext btc )
{
  SSaverContext* sc = &(((ObjectFootPrint*)AllocateObject( btc->m_Pool ))->m_SaverContext);
  sc->m_Tree      = btc;
  sc->m_Extra     = 0x0;
  sc->m_Allocator = btc->m_Allocator;
  StringBufferInit( sc->m_Allocator, &sc->m_Buffer, 8 * 1024 );
  return sc;
}

void SaverContextDestroy( SaverContext sc )
{
  StringBufferDestroy( &sc->m_Buffer );
  FreeObject( sc->m_Tree->m_Pool, sc );
}

