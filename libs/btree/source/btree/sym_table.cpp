/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "sym_table.h"
#include <algorithm>
#include <stdlib.h>

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

void SymbolTableGrow( SymbolTable* st )
{
  st->m_Capacity += 512;
  const size_t alloc_size = sizeof(NamedSymbol) * st->m_Capacity;
  NamedSymbol* new_buffer = (NamedSymbol*)st->m_Alloc( alloc_size );

  memset( new_buffer, 0xcd, alloc_size );

  if( st->m_Size > 0 )
    memcpy( new_buffer, st->m_Symbols, sizeof(NamedSymbol) * (st->m_Size) );

  if( st->m_Symbols )
    st->m_Free( st->m_Symbols );

  st->m_Symbols = new_buffer;
}

void SymbolTableInit( SymbolTable* st, AllocateMemoryFunc alloc,
  FreeMemoryFunc free )
{
  st->m_Capacity    = 0;
  st->m_Size        = 0;
  st->m_Symbols     = 0;
  st->m_Alloc       = alloc;
  st->m_Free        = free;
}

void SymbolTableDestroy( SymbolTable* st )
{
  st->m_Free( st->m_Symbols );
  memset( st, 0xdeadbeef, sizeof(SymbolTable) );
}

NamedSymbol* SymbolTableFind( SymbolTable* st, hash_t id )
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

void SymbolTableInsert( SymbolTable* st, const NamedSymbol& s )
{
  if( st->m_Size == st->m_Capacity )
    SymbolTableGrow( st );

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
