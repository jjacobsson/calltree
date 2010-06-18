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
BehaviorTreeContext create_bt_context( Allocator& allocator )
{
  ObjectPoolSetup ops;
  ops.m_Allocator = allocator;
  ops.m_BlockSize = 4096;
  ops.m_TypeSize = (mem_size_t)sizeof(ObjectFootPrint);
  ObjectPool* op = create_object_pool( &ops );

  SBehaviorTreeContext* btc =
      &(((ObjectFootPrint*)allocate_object( op ))->m_BTContext);
  btc->m_Pool = op;
  btc->m_Allocator = allocator;
  btc->m_Includes = 0x0;
  btc->m_Options = 0x0;
  btc->m_NodeId = 0;

  init( &btc->m_StringTable, btc->m_Allocator );
  init( &btc->m_SymbolTable, btc->m_Allocator );

  return btc;
}

void destroy( BehaviorTreeContext btc )
{
  if( !btc )
    return;

  destroy( &btc->m_StringTable );
  destroy( &btc->m_SymbolTable );

  ObjectPool* op = btc->m_Pool;
  free_object( op, btc );
  destroy( op );
}

const char* register_string( BehaviorTreeContext btc, const char* str )
{
  if( !str )
    return 0x0;
  return register_string( &btc->m_StringTable, str, hashlittle( str ) );
}

const char* register_string( BehaviorTreeContext btc, const char* str,
  hash_t hash )
{
  if( !str )
    return 0x0;
  return register_string( &btc->m_StringTable, str, hash );
}

void* allocate_object( BehaviorTreeContext btc )
{
  return allocate_object( btc->m_Pool );
}

void free_object( BehaviorTreeContext btc, void* object )
{
  free_object( btc->m_Pool, object );
}

void register_symbol( BehaviorTreeContext btc, const NamedSymbol& s )
{
  insert( &btc->m_SymbolTable, s );
}

void remove_symbol( BehaviorTreeContext btc, hash_t hash )
{
  erase( &btc->m_SymbolTable, hash );
}

NamedSymbol* find_symbol( BehaviorTreeContext btc, const char* name )
{
  return find( &btc->m_SymbolTable, hashlittle( name ) );
}

NamedSymbol* find_symbol( BehaviorTreeContext btc, hash_t hash )
{
  return find( &btc->m_SymbolTable, hash );
}

NamedSymbol* access_symbols( BehaviorTreeContext btc, int* count )
{
  *count = btc->m_SymbolTable.m_Size;
  return btc->m_SymbolTable.m_Symbols;
}

void add_include( BehaviorTreeContext btc, const Include& include )
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

  Include* i = &(((ObjectFootPrint*)allocate_object( btc->m_Pool ))->m_Include);
  *i = include;
  i->m_Hash = h;
  i->m_Next = 0x0;
  i->m_UserData = 0x0;

  if( l )
    l->m_Next = i;
  else
    btc->m_Includes = i;
}

Include* create_include( BehaviorTreeContext btc, const char* path_in )
{
  const char* path = register_string( btc, path_in );
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

  Include* i = &(((ObjectFootPrint*)allocate_object( btc->m_Pool ))->m_Include);
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

Include* get_first_include( BehaviorTreeContext btc )
{
  return btc->m_Includes;
}

void release_include( BehaviorTreeContext btc, Include* i )
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
  free_object( btc, i );
}

BehaviorTree* look_up_behavior_tree( BehaviorTreeContext btc, Identifier* id )
{
  {
    NamedSymbol* s = find_symbol( btc, id->m_Hash );
    if( s )
    {
      if( s->m_Type != E_ST_TREE )
        return 0x0;
      return s->m_Symbol.m_Tree;
    }
  }
  BehaviorTree* t = (BehaviorTree*)allocate_object( btc );
  init( t );
  clone( btc, &t->m_Id, id );

  NamedSymbol s;
  s.m_Type = E_ST_TREE;
  s.m_Symbol.m_Tree = t;
  register_symbol( btc, s );

  return t;
}

Decorator* look_up_decorator( BehaviorTreeContext btc, Identifier* id )
{
  {
    NamedSymbol* s = find_symbol( btc, id->m_Hash );
    if( s )
    {
      if( s->m_Type != E_ST_DECORATOR )
        return 0x0;
      return s->m_Symbol.m_Decorator;
    }
  }
  Decorator* d = (Decorator*)allocate_object( btc );
  init( d );
  clone( btc, &d->m_Id, id );

  NamedSymbol s;
  s.m_Type = E_ST_DECORATOR;
  s.m_Symbol.m_Decorator = d;
  register_symbol( btc, s );

  return d;
}

Action* look_up_action( BehaviorTreeContext btc, Identifier* id )
{
  {
    NamedSymbol* s = find_symbol( btc, id->m_Hash );
    if( s )
    {
      if( s->m_Type != E_ST_ACTION )
        return 0x0;
      return s->m_Symbol.m_Action;
    }
  }
  Action* a = (Action*)allocate_object( btc );
  init( a );
  clone( btc, &a->m_Id, id );

  NamedSymbol s;
  s.m_Type = E_ST_ACTION;
  s.m_Symbol.m_Action = a;
  register_symbol( btc, s );

  return a;
}

Parameter* look_up_type( BehaviorTreeContext btc, Identifier* id )
{
  {
    NamedSymbol* s = find_symbol( btc, id->m_Hash );
    if( s )
    {
      if( s->m_Type != E_ST_TYPE )
        return 0x0;
      return s->m_Symbol.m_Type;
    }
  }
  Parameter* p = (Parameter*)allocate_object( btc );
  init( p );
  clone( btc, &p->m_Id, id );

  NamedSymbol s;
  s.m_Type = E_ST_TYPE;
  s.m_Symbol.m_Type = p;
  register_symbol( btc, s );
  return p;
}

Parameter* get_options( BehaviorTreeContext btc )
{
  return btc->m_Options;
}

ParserContext create_parser_context( BehaviorTreeContext btc )
{
  SParserContext* pc =
      &(((ObjectFootPrint*)allocate_object( btc->m_Pool ))->m_ParserContext);

  pc->m_Tree = btc;
  pc->m_LineNo = 0;
  pc->m_Extra = 0x0;
  pc->m_Current = 0x0;
  pc->m_Allocator = btc->m_Allocator;
  init( pc->m_Allocator, &pc->m_Parsed );
  init( pc->m_Allocator, &pc->m_Original );
  memset( &pc->m_Funcs, 0, sizeof(ParserContextFunctions) );
  return pc;
}

void destroy( ParserContext pc )
{
  destroy( &pc->m_Parsed );
  destroy( &pc->m_Original );
  free_object( pc->m_Tree->m_Pool, pc );
}

SaverContext create_saver_context( BehaviorTreeContext btc )
{
  SSaverContext* sc =
      &(((ObjectFootPrint*)allocate_object( btc->m_Pool ))->m_SaverContext);
  sc->m_Tree = btc;
  sc->m_Extra = 0x0;
  sc->m_Allocator = btc->m_Allocator;
  init( sc->m_Allocator, &sc->m_Buffer, 8 * 1024 );
  return sc;
}

void destroy( SaverContext sc )
{
  destroy( &sc->m_Buffer );
  free_object( sc->m_Tree->m_Pool, sc );
}

