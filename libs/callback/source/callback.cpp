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

namespace cb
{

int run_program( CallbackProgram* info )
{

  typedef unsigned int type;

  CallbackHandler ch = info->m_Callback;
  DebugHandler dh    = info->m_Debug;

  ProgramHeader* ph  = (ProgramHeader*)(info->m_Program);
  // Get the instruction array
  Instruction* inst  = (Instruction*)((type)ph + ph->m_Inst);
  // Setup the context ptr
  Context* ctx       = (Context*)info->m_Memory;
  // Setup the data register
  ctx->r[eds] = (type)ph + ph->m_Data;
  // Setup the function table register
  ctx->r[eft] = (type)ph + ph->m_Funt;
  // Setup memory register
  ctx->r[ems] = (type)ctx + ph->m_Bss;
  // Setup the stack register
  ctx->r[esp] = (type)ctx + ph->m_Stack;
  // Setup the call stack register
  ctx->r[efs] = (type)ctx + ph->m_CStack;
  // Setup instruction pointer
  ctx->r[eip] = 0;

  // Increment the frame counter
  ctx->r[efc]++;

  Instruction* i;

start_label:

  i = &inst[ctx->r[eip]];
  ctx->r[eip]++;
  ctx->r[eic]++;

  switch( i->i )
  {
  case inop:
    break;
  case icall:
    *(type*)ctx->r[efs] = ctx->r[eip];
    ctx->r[efs] += sizeof(type);
    ctx->r[eip] = ((FunctionTableEntry*)ctx->r[eft])[ctx->r[i->a1]].m_Start;
    break;
  case iret:
    ctx->r[efs] -= sizeof(type);
    ctx->r[eip] = *(type*)ctx->r[efs];
    break;
  case iccall:
    if( ch )
      ctx->r[err] =
        ch(
          (CallData*)(ctx->r[i->a1]),
          info->m_UserData
        );
    break;
  case idcall:
    if( dh )
      dh(
        info,
        (DebugData*)(ctx->r[i->a1]),
        ctx
      );
    break;
  case imov:
    ctx->r[i->a1] = ctx->r[i->a2];
    break;
  case iload:
    ctx->r[i->a1] = *(type*)(ctx->r[i->a2] + (sizeof(type)*i->a3));
    break;
  case istore:
    *(type*)(ctx->r[i->a1] + (sizeof(type)*i->a2)) = ctx->r[i->a3];
    break;
  case ipush:
    *(type*)(ctx->r[esp]) = ctx->r[i->a1];
    ctx->r[esp] += sizeof(type);
    break;
  case ipop:
    ctx->r[esp] -= sizeof(type);
    ctx->r[i->a1] = *(type*)ctx->r[esp];
    break;
  case ishftl:
    ctx->r[i->a1] = ctx->r[i->a2] << i->a3;
    break;
  case ishftr:
    ctx->r[i->a1] = ctx->r[i->a2] >> i->a3;
    break;
  case iand:
    ctx->r[i->a1] = ctx->r[i->a2] & ctx->r[i->a3];
    break;
  case ior:
    ctx->r[i->a1] = ctx->r[i->a2] | ctx->r[i->a3];
    break;
  case ixor:
    ctx->r[i->a1] = ctx->r[i->a2] ^ ctx->r[i->a3];
    break;
  case isetl:
    ctx->r[i->a1] = (((type)i->a2)<<8)|(type)i->a3;
    break;
  case iseth:
    ctx->r[i->a1] = (((type)i->a2)<<24)|(((type)i->a3)<<16);
    break;
  case iandl:
    ctx->r[i->a1] &= (((type)i->a2)<<8)|(type)i->a3;
    break;
  case iandh:
    ctx->r[i->a1] &= (((type)i->a2)<<24)|(((type)i->a3)<<16);
    break;
  case iorl:
    ctx->r[i->a1] |= (((type)i->a2)<<8)|(type)i->a3;
    break;
  case iorh:
    ctx->r[i->a1] |= (((type)i->a2)<<24)|(((type)i->a3)<<16);
    break;
  case ixorl:
    ctx->r[i->a1] ^= (((type)i->a2)<<8)|(type)i->a3;
    break;
  case ixorh:
    ctx->r[i->a1] ^= (((type)i->a2)<<24)|(((type)i->a3)<<16);
    break;
  case iexit:
    goto exit_label;
    break;
  }
  goto start_label;

exit_label:
  return 0;
}

}
