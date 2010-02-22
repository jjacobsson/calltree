/*******************************************************************************
 * Copyright (c) 2010-02-22 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#ifndef BTREE_INTERNAL_H_INCLUDED
#define BTREE_INTERNAL_H_INCLUDED

#include <btree/btree_data.h>

#include "sym_table.h"
#include "object_pool.h"
#include "string_table.h"

struct SBehaviorTreeContext
{
  StringTable   m_StringTable;
  SymbolTable   m_SymbolTable;
  Allocator     m_Allocator;
  ObjectPool*   m_Pool;
  Include*      m_Includes;
};

#endif /*BTREE_INTERNAL_H_INCLUDED*/
