/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include <btree/btree.h>

#include <other/lookup3.h>

#include <btree/btree_data.h>
#include <btree/btree_func.h>
#include "../parser/common.h"

#include "sym_table.h"
#include "object_pool.h"
#include "string_table.h"

struct StandardAllocator
{
    static void* Alloc( size_t size )
    {
        return malloc( size );
    }

    static void Free( void* ptr )
    {
        return free( ptr );
    }
};

struct HashPredicate
{
    inline bool operator() ( const hash_t l, const hash_t r ) const
    {
        return l < r;
    }
};

struct IntPredicate
{
    inline bool operator() ( int l, int r ) const
    {
        return l < r;
    }
    inline bool Equals( int l, int r ) const
    {
        return l == r;
    }
};

template< typename T >
struct HasIdPredicate
{
    inline bool operator() ( const T* l, const hash_t r ) const
    {
        return l->m_Id.m_Hash < r;
    }
    inline bool operator() ( const hash_t l, const T* r ) const
    {
        return l < r->m_Id.m_Hash;
    }
    inline bool operator() ( const T* l, const Identifier& r ) const
    {
        return l->m_Id.m_Hash < r.m_Hash;
    }
    inline bool operator() ( const Identifier& l, const T* r ) const
    {
        return l.m_Hash < r->m_Id.m_Hash;
    }
    inline bool operator() ( const T* l, const T* r ) const
    {
        return l->m_Id.m_Hash < r->m_Id.m_Hash;
    }

    inline bool Equals( const T* l, const Identifier& r ) const
    {
        return l->m_Id.m_Hash == r.m_Hash;
    }

    inline bool Equals( const T* l, const T* r ) const
    {
        return l == r;
    }

};

typedef TSymbolTable<Action*, HasIdPredicate<Action> > ActionTable;
typedef TSymbolTable<Decorator*, HasIdPredicate<Decorator> > DecoratorTable;

struct SBehaviorTreeContext
{
  BehaviorTreeContextSetup  m_Setup;
  StringTable               m_StringTable;
  BehaviorTree*             m_Trees;
  ObjectPool*               m_Pool;
  ActionTable*              m_ActionTable;
  DecoratorTable*           m_DecoratorTable;
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
  btc->m_ActionTable    = new ActionTable;
  btc->m_DecoratorTable = new DecoratorTable;
  btc->m_Setup          = *btcs;

  StringTableInit( &btc->m_StringTable );
  btc->m_StringTable.m_Alloc    = btcs->m_Alloc;
  btc->m_StringTable.m_Free     = btcs->m_Free;

  return btc;
}

void BehaviorTreeContextDestroy( BehaviorTreeContext btc )
{
  delete btc->m_ActionTable;
  delete btc->m_DecoratorTable;
  DestroyObjectPool( btc->m_Pool );
  StringTableDestroy( &btc->m_StringTable );
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
