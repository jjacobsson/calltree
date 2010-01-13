/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef SYM_TABLE_H_
#define SYM_TABLE_H_

#include <btree/btree_data.h>

struct SymbolTable
{
  AllocateMemoryFunc    m_Alloc;
  FreeMemoryFunc        m_Free;
  NamedSymbol*          m_Symbols;
  int                   m_Size;
  int                   m_Capacity;
};

void SymbolTableInit( SymbolTable*, AllocateMemoryFunc alloc, FreeMemoryFunc free  );
void SymbolTableDestroy( SymbolTable* );

NamedSymbol* SymbolTableFind( SymbolTable*, hash_t );
void SymbolTableInsert( SymbolTable*, const NamedSymbol& );

#endif /*SYM_TABLE_H_*/
