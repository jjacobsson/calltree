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

#include "parser/parser.h"
#include "saver/saver.h"

struct SBehaviorTreeContext
{
  StringTable   m_StringTable;
  SymbolTable   m_SymbolTable;
  Allocator     m_Allocator;
  ObjectPool*   m_Pool;
  Include*      m_Includes;
  Parameter*    m_Options;
  unsigned int  m_NodeId;
};

union ObjectFootPrint
{
  Parameter            m_Variable;
  Action               m_Action;
  Decorator            m_Decorator;
  Node                 m_Node;
  BehaviorTree         m_Tree;
  SBehaviorTreeContext m_BTContext;
  SParserContext       m_ParserContext;
  Include              m_Include;
  SSaverContext        m_SaverContext;
};

#endif /*BTREE_INTERNAL_H_INCLUDED*/
