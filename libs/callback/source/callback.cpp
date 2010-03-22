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

bool exi_flag_set( unsigned int f )
{
  return (f & E_EXIT_SCOPE) != 0;
}

bool ent_flag_set( unsigned int f )
{
  return (f & E_ENTER_SCOPE) != 0;
}

int run_program( CallbackProgram* info )
{
  ProgramHeader* ph = (ProgramHeader*)(info->m_Program);
  Instruction* i = (Instruction*)((char*)(info->m_Program)
      + sizeof(ProgramHeader));
  char* bss = (char*)info->m_bss;
  char* data = ((char*)(i)) + (sizeof(Instruction) * ph->m_IC);
  BssHeader* bh = (BssHeader*)(bss);
  CallbackHandler ch = info->m_Callback;
  DebugHandler dh = info->m_Debug;

  start: const Instruction& inst = i[bh->m_IP];
  ++bh->m_IP;
  ++bh->m_IC;
  switch( inst.m_I )
  {
  case INST_CALL_DEBUG_FN:
    if( dh )
      dh( info, (DebugInformation*)(&bss[inst.m_A1]), bh, info->m_UserData );
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
      bh->m_IP = inst.m_A1;
    break;
  case INST_JABC_R_DIFF_C:
    if( bh->m_RE != inst.m_A2 )
      bh->m_IP = inst.m_A1;
    break;
  case INST_JABC_C_EQUA_B:
    if( inst.m_A2 == *((int*)&(bss[inst.m_A3])) )
      bh->m_IP = inst.m_A1;
    break;
  case INST_JABC_C_DIFF_B:
    if( inst.m_A2 != *((int*)&(bss[inst.m_A3])) )
      bh->m_IP = inst.m_A1;
    break;
  case INST_JABB_C_EQUA_B:
    if( inst.m_A2 == *((int*)&(bss[inst.m_A3])) )
      bh->m_IP = *((int*)&(bss[inst.m_A1]));
    break;
  case INST_JABB_C_DIFF_B:
    if( inst.m_A2 != *((int*)&(bss[inst.m_A3])) )
      bh->m_IP = *((int*)&(bss[inst.m_A1]));
    break;
  case INST_JABB_B_EQUA_B:
    if( *((int*)&(bss[inst.m_A2])) == *((int*)&(bss[inst.m_A3])) )
      bh->m_IP = *((int*)&(bss[inst.m_A1]));
    break;
  case INST_JABB_B_DIFF_B:
    if( *((int*)&(bss[inst.m_A2])) != *((int*)&(bss[inst.m_A3])) )
      bh->m_IP = *((int*)&(bss[inst.m_A1]));
    break;
  case INST_JABC_CONSTANT:
    bh->m_IP = inst.m_A1;
    break;
  case INST_JREC_CONSTANT:
    bh->m_IP += inst.m_A1;
    break;
  case INST_JABB_BSSVALUE:
    bh->m_IP = *((int*)&(bss[inst.m_A1]));
    break;
  case INST_JREB_BSSVALUE:
    bh->m_IP += *((int*)&(bss[inst.m_A1]));
    break;
  case INST_JABC_S_C_IN_B:
    bh->m_IP = inst.m_A1;
    *((int*)&(bss[inst.m_A2])) = inst.m_A3;
    break;
  case INST_JREC_S_C_IN_B:
    bh->m_IP += inst.m_A1;
    *((int*)&(bss[inst.m_A2])) = inst.m_A3;
    break;
  case INST_JABB_S_C_IN_B:
    bh->m_IP = *((int*)&(bss[inst.m_A1]));
    *((int*)&(bss[inst.m_A2])) = inst.m_A3;
    break;
  case INST_JREB_S_C_IN_B:
    bh->m_IP += *((int*)&(bss[inst.m_A1]));
    *((int*)&(bss[inst.m_A2])) = inst.m_A3;
    break;
  case INST__STORE_R_IN_B:
    *((int*)&(bss[inst.m_A1])) = bh->m_RE;
    break;
  case INST__STORE_B_IN_R:
    bh->m_RE = *((int*)&(bss[inst.m_A1]));
    break;
  case INST__STORE_C_IN_B:
    *((int*)&(bss[inst.m_A1])) = inst.m_A2;
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
  case INST_LOAD_REGISTRY:
    bh->m_R[inst.m_A1] = (((int)inst.m_A2) << 16) + inst.m_A3;
    break;
  case INST_______SUSPEND:
    bh->m_IP = 0;
    goto exit;
    break;
  }
  goto start;
  exit:

  return bh->m_RE;
}

}
