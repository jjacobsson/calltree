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

#ifndef SYM_TABLE_H_
#define SYM_TABLE_H_

#include <btree/btree_data.h>

struct SymbolTable
{
  Allocator    m_Allocator;
  NamedSymbol* m_Symbols;
  int          m_Size;
  int          m_Capacity;
};

void SymbolTableInit( SymbolTable*, Allocator );
void SymbolTableDestroy( SymbolTable* );

NamedSymbol* SymbolTableFind( SymbolTable*, hash_t );
void SymbolTableInsert( SymbolTable*, const NamedSymbol& );

#endif /*SYM_TABLE_H_*/
