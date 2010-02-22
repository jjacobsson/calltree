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

#ifndef STRING_TABLE_H_
#define STRING_TABLE_H_

#include <btree/btree_data.h>

struct StringTableLookup;
struct StringTableBlock;

struct StringTable
{
  int                m_BlockSize;
  int                m_LookupGrow;
  int                m_LookupSize;
  int                m_LookupCapacity;
  StringTableLookup* m_LookupTable;
  StringTableBlock*  m_FirstBlock;
  Allocator          m_Allocator;
};

void StringTableInit( StringTable*, Allocator );
void StringTableDestroy( StringTable* );

const char* StringTableRegisterString( StringTable*, const char*, hash_t );

#endif /*STRING_TABLE_H_*/
