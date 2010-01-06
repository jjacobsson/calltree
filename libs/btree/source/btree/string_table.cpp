/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "string_table.h"
#include <string.h>
#include <algorithm>

struct StringTableBlock
{
  StringTableBlock* m_Next;
  int               m_Used;
  char*             m_Text;
};

struct StringTableLookup
{
  hash_t    m_Hash;
  char*     m_Str;
};

class StringTableLookupPred
{
public:
  bool operator()( const StringTableLookup& l, const StringTableLookup& r ) const
  {
    return l.m_Hash < r.m_Hash;
  }
};

void StringTableGrowHashTable( StringTable* st )
{
  int alloc = sizeof(StringTableLookup) * st->m_LookupCapacity
      + sizeof(StringTableLookup) * st->m_LookupGrow;
  StringTableLookup* n = (StringTableLookup*)st->m_Alloc( alloc );
  if( st->m_LookupTable )
    memcpy( n, st->m_LookupTable, sizeof(StringTableLookup) * st->m_LookupCapacity );

  StringTableLookup* o = st->m_LookupTable;
  st->m_LookupTable = n;
  st->m_LookupCapacity += st->m_LookupGrow;

  st->m_Free( o );
}

void StringTableHashInsert( StringTable* st, StringTableLookup& l )
{
  if( st->m_LookupSize == st->m_LookupCapacity )
    StringTableGrowHashTable( st );

  StringTableLookupPred pred;

  StringTableLookup* it = std::lower_bound( st->m_LookupTable,
    st->m_LookupTable + st->m_LookupSize, l, pred );

  if( it != st->m_LookupTable + st->m_LookupSize )
  {
    int count = (st->m_LookupSize * sizeof(StringTableLookup)) - ((int)it
        - (int)st->m_LookupTable);
    memmove( it + 1, it, count );
  }

  it->m_Hash = l.m_Hash;
  it->m_Str = l.m_Str;
  st->m_LookupSize++;
}

StringTableBlock* StringTableAllocateBlock( StringTable* st, int minimum = 0 )
{
  int alloc;

  if( st->m_BlockSize < minimum )
    alloc = sizeof(StringTableBlock) + minimum;
  else
    alloc = sizeof(StringTableBlock) + st->m_BlockSize;

  StringTableBlock* c = (StringTableBlock*)st->m_Alloc( alloc );
  c->m_Next = 0x0;
  c->m_Used = 0;
  c->m_Text = (char*)(c) + sizeof(StringTableBlock);

  return c;
}

void StringTableInit( StringTable* st )
{
  st->m_BlockSize = 4096;
  st->m_LookupCapacity = 0;
  st->m_LookupGrow = 256;
  st->m_LookupSize = 0;
  st->m_LookupTable = 0x0;
  st->m_FirstBlock = 0x0;
  st->m_Alloc = 0x0;
  st->m_Free = 0x0;
}

void StringTableDestroy( StringTable* st )
{
  st->m_Free( st->m_LookupTable );
  while( st->m_FirstBlock )
  {
    StringTableBlock* t = st->m_FirstBlock->m_Next;
    st->m_Free( st->m_FirstBlock );
    st->m_FirstBlock = t;
  }
}

const char* StringTableRegisterString( StringTable* st, const char* str,
  hash_t hash )
{
  StringTableLookupPred pred;
  StringTableLookup hl;
  hl.m_Hash = hash;

  if( st->m_LookupTable )
  {
    StringTableLookup* it = std::lower_bound( st->m_LookupTable,
      st->m_LookupTable + st->m_LookupSize, hl, pred );
    if( it != st->m_LookupTable + st->m_LookupSize && it->m_Hash == hash )
      return it->m_Str;
  }

  int l = strlen( str );
  if( !st->m_FirstBlock )
  {
    st->m_FirstBlock = StringTableAllocateBlock( st, l + 1 );
  }
  else if( st->m_BlockSize - st->m_FirstBlock->m_Used < l + 1 )
  {
    StringTableBlock* c = StringTableAllocateBlock( st, l + 1 );
    c->m_Next = st->m_FirstBlock;
    st->m_FirstBlock = c;
  }

  hl.m_Str = &(st->m_FirstBlock->m_Text[st->m_FirstBlock->m_Used]);
  StringTableHashInsert( st, hl );

  memcpy( hl.m_Str, str, l + 1 );
  st->m_FirstBlock->m_Used += l + 1;

  return hl.m_Str;
}

const char* StringTableLookupString( hash_t )
{

}

