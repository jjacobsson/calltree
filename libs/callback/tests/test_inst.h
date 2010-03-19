/*******************************************************************************
 * Copyright (c) 2010-03-19 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#ifndef TEST_INST_H_INCLUDED
#define TEST_INST_H_INCLUDED

#include <callback/callback.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace cb;

class VirtualMachineFixture
{
public:

  static const int max_instructions    = 1024;
  static const int max_ftable_entries  = 64;
  static const int max_memory          = 8 * 1024;
  static const int max_data            = 8 * 1024;
  static const int max_jump            = 1 * 1024;
  static const int max_stack           = 1 * 1024;
  static const int max_cstack          = 1 * 1024;
  static const int total_memory =
      sizeof( ProgramHeader ) +
      max_instructions * sizeof( Instruction ) +
      max_data +
      max_jump +
      max_ftable_entries * sizeof( FunctionTableEntry ) +
      sizeof( Context ) +
      max_memory +
      max_stack +
      max_cstack;

  VirtualMachineFixture()
  {
    m_Memory = (unsigned char*)malloc( total_memory );
    memset( m_Memory, 0, total_memory );
    ph = (ProgramHeader*)m_Memory;
    ph->m_Inst   = sizeof( ProgramHeader );
    ph->m_Data   = ph->m_Inst + sizeof( Instruction ) * max_instructions;
    ph->m_Funt   = ph->m_Data + max_data;
    ph->m_Jump   = ph->m_Funt + sizeof(FunctionTableEntry)*max_ftable_entries;
    ph->m_Bss    = sizeof( Context );
    ph->m_Stack  = ph->m_Bss + max_memory;
    ph->m_CStack = ph->m_Stack + max_stack;

    inst = (Instruction*)((unsigned int)ph + ph->m_Inst);
    data = (unsigned int*)((unsigned int)ph + ph->m_Data);
    funt = (FunctionTableEntry*)((unsigned int)ph + ph->m_Funt);

    ctx    = (Context*)((unsigned int)funt + sizeof( FunctionTableEntry ) * max_ftable_entries);
    bss    = (unsigned int*)((unsigned int)ctx + ph->m_Bss);
    stack  = (unsigned int*)((unsigned int)ctx + ph->m_Stack);
    cstack = (unsigned int*)((unsigned int)ctx + ph->m_CStack);

    cp.m_Program  = (void*)m_Memory;
    cp.m_Memory   = (void*)ctx;
    cp.m_UserData = 0x0;
    cp.m_Callback = 0x0;
    cp.m_Debug    = 0x0;
  }

  ~VirtualMachineFixture()
  {
    free( m_Memory );
  }

  unsigned char* m_Memory;

  ProgramHeader* ph;
  Instruction* inst;
  unsigned int* data;
  FunctionTableEntry* funt;
  unsigned int* jump;
  Context* ctx;
  unsigned int* bss;
  unsigned int* stack;
  unsigned int* cstack;

  CallbackProgram cp;

};

#endif /* TEST_INST_H_INCLUDED */
