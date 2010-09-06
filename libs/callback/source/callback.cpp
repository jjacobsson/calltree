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

#include <callback/callback.h>
#include <callback/instructions.h>

namespace callback
{

bool act_flag_set( unsigned int f, unsigned int a )
{
  return ((unsigned int)(f & 0x000f)) == a;
}

bool std_flag_set( unsigned int f )
{
  return (f & E_STANDARD_NODE) != 0;
}

bool com_flag_set( unsigned int f )
{
  return (f & E_COMPOSITE_NODE) != 0;
}

bool exi_flag_set( unsigned int f )
{
  return (f & E_EXIT_SCOPE) != 0;
}

bool ent_flag_set( unsigned int f )
{
  return (f & E_ENTER_SCOPE) != 0;
}

#ifdef SPU
  #define ASM_BREAKPOINT { __asm__ volatile("stopd 0, 0, 1"); }
  #define CHECKED_IP_ASSIGNMENT( X ) { if( (int)ph->m_IC <= X ) ASM_BREAKPOINT else { bh->m_IP = X; } }
  #define CHECKED_IP_ADDITION( X ) { int Temp = bh->m_IP + (int)X; if( (int)ph->m_IC <= Temp ) ASM_BREAKPOINT else { bh->m_IP = Temp; } }
#else
  #define CHECKED_IP_ASSIGNMENT( X ) { bh->m_IP = X; }
  #define CHECKED_IP_ADDITION( X ) { bh->m_IP += X; }
#endif


int run_program( CallbackProgram* info )
{
  ProgramHeader* ph = (ProgramHeader*)(info->m_Program);
  Instruction* i = (Instruction*)((char*)(info->m_Program)
      + sizeof(ProgramHeader));
  char* bss = (char*)(info->m_bss) + sizeof(BssHeader);
  char* data = ((char*)(i)) + (sizeof(Instruction) * ph->m_IC);
  BssHeader* bh = (BssHeader*)info->m_bss;
  CallbackHandler ch = info->m_Callback;
  DebugHandler dh = info->m_Debug;

  start:
  const Instruction& inst = i[bh->m_IP];
  ++bh->m_IP;
  ++bh->m_IC;
  switch( inst.m_I )
  {
  case INST_CALL_DEBUG_FN:
    if( dh )
      dh( info, (DebugInformation*)bh->m_R, bh, info->m_UserData );
    break;
  case INST_CALL_CONS_FUN:
    ch( bh->m_R[inst.m_A1], ACT_CONSTRUCT, (void*)bh->m_R[inst.m_A2],
      (void**)bh->m_R[inst.m_A3], info->m_UserData );
    bh->m_R[inst.m_A1] = 0;
    bh->m_R[inst.m_A2] = 0;
    bh->m_R[inst.m_A3] = 0;
    break;
  case INST_CALL_EXEC_FUN:
    bh->m_RE = ch( bh->m_R[inst.m_A1], ACT_EXECUTE, (void*)bh->m_R[inst.m_A2],
      (void**)bh->m_R[inst.m_A3], info->m_UserData );
    bh->m_R[inst.m_A1] = 0;
    bh->m_R[inst.m_A2] = 0;
    bh->m_R[inst.m_A3] = 0;
    break;
  case INST_CALL_DEST_FUN:
    ch( bh->m_R[inst.m_A1], ACT_DESTRUCT, (void*)bh->m_R[inst.m_A2],
      (void**)bh->m_R[inst.m_A3], info->m_UserData );
    bh->m_R[inst.m_A1] = 0;
    bh->m_R[inst.m_A2] = 0;
    bh->m_R[inst.m_A3] = 0;
    break;

  case INST_CALL_PRUN_FUN:
    bh->m_RE = ch( bh->m_R[inst.m_A1], ACT_PRUNE, (void*)bh->m_R[inst.m_A2],
      (void**)bh->m_R[inst.m_A3], info->m_UserData );
    bh->m_R[inst.m_A1] = 0;
    bh->m_R[inst.m_A2] = 0;
    bh->m_R[inst.m_A3] = 0;
    break;
  case INST_CALL_MODI_FUN:
    bh->m_RE = ch( bh->m_R[inst.m_A1], ACT_MODIFY, (void*)bh->m_R[inst.m_A2],
      (void**)bh->m_R[inst.m_A3], info->m_UserData );
    bh->m_R[inst.m_A1] = 0;
    bh->m_R[inst.m_A2] = 0;
    bh->m_R[inst.m_A3] = 0;
    break;
  case INST_JABC_R_EQUA_C:
    if( bh->m_RE == inst.m_A2 )
      CHECKED_IP_ASSIGNMENT( inst.m_A1 );
    break;
  case INST_JABC_R_DIFF_C:
    if( bh->m_RE != inst.m_A2 )
      CHECKED_IP_ASSIGNMENT( inst.m_A1 );
    break;
  case INST_JABC_C_EQUA_B:
    if( inst.m_A2 == *((int*)&(bss[inst.m_A3])) )
      CHECKED_IP_ASSIGNMENT( inst.m_A1 );
    break;
  case INST_JABC_C_DIFF_B:
    if( inst.m_A2 != *((int*)&(bss[inst.m_A3])) )
      CHECKED_IP_ASSIGNMENT( inst.m_A1 );
    break;
  case INST_JABB_C_EQUA_B:
    if( inst.m_A2 == *((int*)&(bss[inst.m_A3])) )
      CHECKED_IP_ASSIGNMENT( *((int*)&(bss[inst.m_A1])) );
    break;
  case INST_JABB_C_DIFF_B:
    if( inst.m_A2 != *((int*)&(bss[inst.m_A3])) )
      CHECKED_IP_ASSIGNMENT( *((int*)&(bss[inst.m_A1])) );
    break;
  case INST_JABB_B_EQUA_B:
    if( *((int*)&(bss[inst.m_A2])) == *((int*)&(bss[inst.m_A3])) )
      CHECKED_IP_ASSIGNMENT( *((int*)&(bss[inst.m_A1])) );
    break;
  case INST_JABB_B_DIFF_B:
    if( *((int*)&(bss[inst.m_A2])) != *((int*)&(bss[inst.m_A3])) )
      CHECKED_IP_ASSIGNMENT( *((int*)&(bss[inst.m_A1])) );
    break;
  case INST_JABC_CONSTANT:
    CHECKED_IP_ASSIGNMENT( inst.m_A1 );
    break;
  case INST_JREC_CONSTANT:
    CHECKED_IP_ADDITION( inst.m_A1 );
    break;
  case INST_JABB_BSSVALUE:
    CHECKED_IP_ASSIGNMENT( *((int*)&(bss[inst.m_A1])) );
    break;
  case INST_JREB_BSSVALUE:
    CHECKED_IP_ADDITION( *((int*)&(bss[inst.m_A1])) );
    break;
  case INST_JABC_S_C_IN_B:
    CHECKED_IP_ASSIGNMENT( inst.m_A1 );
    *((int*)&(bss[inst.m_A2])) = inst.m_A3;
    break;
  case INST_JREC_S_C_IN_B:
    CHECKED_IP_ADDITION( inst.m_A1 );
    *((int*)&(bss[inst.m_A2])) = inst.m_A3;
    break;
  case INST_JABB_S_C_IN_B:
    CHECKED_IP_ASSIGNMENT( *((int*)&(bss[inst.m_A1])) );
    *((int*)&(bss[inst.m_A2])) = inst.m_A3;
    break;
  case INST_JREB_S_C_IN_B:
    CHECKED_IP_ADDITION( *((int*)&(bss[inst.m_A1])) );
    *((int*)&(bss[inst.m_A2])) = inst.m_A3;
    break;
  case INST__STORE_R_IN_B:
    *((int*)&(bss[inst.m_A1])) = bh->m_RE;
    break;
  case INST__STORE_B_IN_R:
    bh->m_RE = *((int*)&(bss[inst.m_A1]));
    break;
  case INST__STORE_C_IN_B:
    *((int*)&(bss[inst.m_A1])) = (((int)inst.m_A3)<<16) | ((int)inst.m_A2);
    break;
  case INST__STORE_B_IN_B:
    *((int*)&(bss[inst.m_A1])) = *((int*)&(bss[inst.m_A2]));
    break;
  case INST__STORE_C_IN_R:
    bh->m_RE = inst.m_A1;
    break;
  case INST_STORE_PD_IN_B:
    *(int*)(&bss[inst.m_A1]) = (int)(&data[inst.m_A2]);
    break;
  case INST_STORE_PB_IN_R:
    bh->m_R[inst.m_A1] = (int)(&bss[inst.m_A2]);
    break;
  case INST__INC_BSSVALUE:
    *((int*)&(bss[inst.m_A1])) += inst.m_A2;
    break;
  case INST__DEC_BSSVALUE:
    *((int*)&(bss[inst.m_A1])) += inst.m_A2;
    break;
  case INST__SET_REGISTRY:
    bh->m_R[inst.m_A1] = (((int)inst.m_A2) << 16) + inst.m_A3;
    break;
  case INST_LOAD_REGISTRY:
    {
      int d = (((int)inst.m_A2) << 16) + ((int)inst.m_A3);
      bh->m_R[inst.m_A1] = (int)(&data[d]);
    }
    break;
  case INST_SCRIPT_C:
    {
      CallFrame* f = (CallFrame*)(bss+inst.m_A2);
      f->m_Bss = bss;
      f->m_IP  = bh->m_IP;
      bss = (char*)(f + 1);
      CHECKED_IP_ASSIGNMENT( inst.m_A1 );
    }
    break;
  case INST_SCRIPT_R:
    {
      CallFrame* f = (CallFrame*)(bss - sizeof(CallFrame));
      bss = f->m_Bss;
      CHECKED_IP_ASSIGNMENT( f->m_IP )
    }
    break;
  case INST_______SUSPEND:
    CHECKED_IP_ASSIGNMENT( 0 );
    goto exit;
    break;
  }
  goto start;
  exit:

  return bh->m_RE;
}

}
