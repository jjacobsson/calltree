/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef STRING_TABLE_H_
#define STRING_TABLE_H_

#include <btree/btree_data.h>
#include <btree/btree_mem.h>

struct StringTableLookup;
struct StringTableBlock;

struct StringTable
{
  int                   m_BlockSize;
  int                   m_LookupGrow;
  int                   m_LookupSize;
  int                   m_LookupCapacity;
  StringTableLookup*    m_LookupTable;
  StringTableBlock*     m_FirstBlock;
  AllocateMemoryFunc    m_Alloc;
  FreeMemoryFunc        m_Free;
};

void StringTableInit( StringTable* );
void StringTableDestroy( StringTable* );

const char* StringTableRegisterString( StringTable*, const char*, hash_t );
const char* StringTableLookupString( StringTable*, hash_t );

#endif /*STRING_TABLE_H_*/
