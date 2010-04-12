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
typedef unsigned char uchar;

typedef void (*AsmFilePrint)( const char* buff, uint size );

struct Function;
struct Program;

struct FunctionEntry
{
  Function* m_F;
  hash_t    m_Id;
};

struct JumpTarget
{
  Function*    m_F; //Owning function (if any)
  unsigned int m_R; //Relative
  unsigned int m_A; //Absolute
};

typedef std::vector<cb::Instruction> InstList;
typedef std::vector<FunctionEntry> FunctionList;
typedef std::vector<JumpTarget> JumpTargets;
typedef std::vector<uint> PrioList;

struct RegState
{
  PrioList  m_Prio;
  uint      m_Stack;
  bool      m_InUse;
};

struct Function
{
  InstList      m_I;
  BehaviorTree* m_T;
  Program*      m_P;
  uint          m_Memory;
  uint          m_Index;
  RegState      m_Reg[cb::gen_reg_count];
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

void count_memory( Function* f );
void count_memory( Program* p );

uint find_function( Program*, const char* );
uint find_function( Program*, hash_t );

void print_asm( AsmFilePrint fp, Program* );

void translate_jump_labels( Program* );

}

#endif /* CB_GEN_H_INCLUDED */
