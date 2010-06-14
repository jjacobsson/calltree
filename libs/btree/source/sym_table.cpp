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

#include "sym_table.h"
#include <algorithm>
#include <stdlib.h>
#include <string.h>

#define ST_ALLOC_MACRO( _size ) st->m_Allocator.m_Alloc(_size)
#define ST_FREE_MACRO( _ptr ) st->m_Allocator.m_Free(_ptr)

struct NamedSymbolPredicate
{
  hash_t GetId( const NamedSymbol& s ) const
  {
    hash_t r = 0x00000000;
    switch( s.m_Type )
    {
    case E_ST_TREE:
      r = s.m_Symbol.m_Tree->m_Id.m_Hash;
      break;
    case E_ST_ACTION:
      r = s.m_Symbol.m_Action->m_Id.m_Hash;
      break;
    case E_ST_DECORATOR:
      r = s.m_Symbol.m_Decorator->m_Id.m_Hash;
      break;
    case E_ST_TYPE:
      r = s.m_Symbol.m_Type->m_Id.m_Hash;
      break;
    case E_ST_UNKOWN:
    case E_MAX_SYMBOL_TYPES:
      /* Warning killers */
      break;
    }
    return r;
  }

  bool operator()( const NamedSymbol& l, const NamedSymbol& r ) const
  {
    hash_t lh = GetId( l ), rh = GetId( r );
    return lh < rh;
  }

  bool operator()( const hash_t& lh, const NamedSymbol& r ) const
  {
    hash_t rh = GetId( r );
    return lh < rh;
  }

  bool operator()( const NamedSymbol& l, const hash_t& rh ) const
  {
    hash_t lh = GetId( l );
    return lh < rh;
  }

  bool Equals( const NamedSymbol& l, const NamedSymbol& r ) const
  {
    hash_t lh = GetId( l ), rh = GetId( r );
    return lh == rh;
  }

} g_NamedSymbolPredicate;

void grow_sym_table( SymbolTable* st )
{
  st->m_Capacity += 512;
  const size_t alloc_size = sizeof(NamedSymbol) * st->m_Capacity;
  NamedSymbol* new_buffer = (NamedSymbol*)ST_ALLOC_MACRO( alloc_size );

  memset( new_buffer, 0xcd, alloc_size );

  if( st->m_Size > 0 )
    memcpy( new_buffer, st->m_Symbols, sizeof(NamedSymbol) * (st->m_Size) );

  if( st->m_Symbols )
    ST_FREE_MACRO( st->m_Symbols );

  st->m_Symbols = new_buffer;
}

void init( SymbolTable* st, Allocator allocator )
{
  st->m_Capacity  = 0;
  st->m_Size      = 0;
  st->m_Symbols   = 0;
  st->m_Allocator = allocator;
}

void destroy( SymbolTable* st )
{
  ST_FREE_MACRO( st->m_Symbols );
  memset( st, 0xdeadbeef, sizeof(SymbolTable) );
}

NamedSymbol* find( SymbolTable* st, hash_t id )
{
  if( !st->m_Symbols )
    return 0x0;

  NamedSymbol* begin = st->m_Symbols;
  NamedSymbol* end = begin + st->m_Size;
  NamedSymbol* s = std::lower_bound( begin, end, id, g_NamedSymbolPredicate );
  if( s != end && g_NamedSymbolPredicate.GetId( *s ) == id )
    return s;

  return 0x0;
}

void insert( SymbolTable* st, const NamedSymbol& s )
{
  if( st->m_Size == st->m_Capacity )
    grow_sym_table( st );

  NamedSymbol* begin = st->m_Symbols;
  NamedSymbol* end = begin + st->m_Size;

  NamedSymbol* it = std::lower_bound( begin, end, s, g_NamedSymbolPredicate );
  if( it == end || !g_NamedSymbolPredicate.Equals( *it, s ) )
  {
    memmove( it + 1, it, (end - it) * sizeof(NamedSymbol) );
    *it = s;
    ++st->m_Size;
  }
}

void erase( SymbolTable* st, hash_t h )
{
  if( !st->m_Symbols )
    return;

  NamedSymbol* begin = st->m_Symbols;
  NamedSymbol* end = begin + st->m_Size;
  NamedSymbol* s = std::lower_bound( begin, end, h, g_NamedSymbolPredicate );
  if( s == end || g_NamedSymbolPredicate.GetId( *s ) != h )
    return;

  if( (end - (s+1)) > 0 )
    memmove( s, s + 1, (end - (s+1)) * sizeof( NamedSymbol ) );

  --st->m_Size;
}

#undef ST_ALLOC_MACRO
#undef ST_FREE_MACRO
