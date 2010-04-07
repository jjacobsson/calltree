/*******************************************************************************
 * Copyright (c) 2010-04-03 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#ifndef CB_GEN_H_INCLUDED
#define CB_GEN_H_INCLUDED

#include <vector>
#include <btree/btree.h>
#include <callback/callback.h>

namespace cb_gen {

typedef unsigned int uint;

struct Function;
struct Program;

struct FunctionEntry
{
  Function* m_Function;
  hash_t    m_Id;
};

struct JumpTarget
{
  Function*    m_Function;
  unsigned int m_Offset;
};

typedef std::vector<Instruction> InstList;
typedef std::vector<FunctionEntry> FunctionList;
typedef std::vector<JumpTarget> JumpTargets;

struct Function
{
  Program* m_P;
  InstList m_I;
  uint     m_Memory;
};

struct Program
{
  FunctionList  m_F;
  InstList      m_I;
  JumpTargets   m_J;
  uint          m_Memory;
};

void init( Program* );
void init( Function* );

void destroy( Program* );
void destroy( Function* );

void generate( BehaviorTreeContext, Program* );

}

#endif /* CB_GEN_H_INCLUDED */
