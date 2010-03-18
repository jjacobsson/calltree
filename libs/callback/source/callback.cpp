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
  Context* ctx       = (Context*)info->m_Memory;
  Instruction* inst  = (Instruction*)((type)info->m_Program + sizeof(ProgramHeader));
  // Setup memory register
  ctx->r[ems]        = (type)((type)info->m_Memory + sizeof( Context ));
  // Setup instruction pointer
  ctx->r[eip]        = 0;
  // Setup the data register
  ctx->r[eds]        = ctx->r[eip] + sizeof(Instruction) * ph->m_IC;

  ctx->r[efc]++;

  Instruction* i;

start_label:

  i = &inst[ctx->r[eip]];
  ctx->r[eip] += sizeof( Instruction );
  ctx->r[eic]++;

  switch( i->i )
  {
  case noop:
    break;
  case call:
    *(type*)ctx->r[efs] = ctx->r[eip];
    ctx->r[efs] += sizeof(type);
    ctx->r[eip] = ((FunctionTableEntry*)ctx->r[eft])
      [
        ((type)i->a1<<16)&
        ((type)i->a2<<8)&
        ((type)i->a3)
      ].m_Start;
    break;
  case ret:
    ctx->r[efs] -= sizeof(type);
    ctx->r[eip] = *(type*)ctx->r[efs];
    break;
  case ccall:
    ctx->r[err] =
      ch(
        (CallData*)(
          ctx->r[i->a1] +
          (sizeof(type)*i->a2) +
          (sizeof(type)*i->a3)
        ),
        info->m_UserData
      );
    break;
  case dcall:
    if( dh )
      dh(
        info,
        (DebugData*)(
          ctx->r[i->a1] +
          (sizeof(type)*i->a2) +
          (sizeof(type)*i->a3)
        ),
        ctx
      );
    break;
  case mov:
    ctx->r[i->a1] = ctx->r[i->a2];
    break;
  case load:
    ctx->r[i->a1] = *(type*)(ctx->r[i->a2] + (sizeof(type)*i->a3));
    break;
  case store:
    *(type*)(ctx->r[i->a1] + (sizeof(type)*i->a2)) = ctx->r[i->a3];
    break;
  case push:
    *(type*)(ctx->r[esp]) = ctx->r[i->a1];
    ctx->r[esp] += sizeof(type);
    break;
  case pop:
    ctx->r[esp] -= sizeof(type);
    ctx->r[i->a1] = *(type*)ctx->r[esp];
    break;
  case exit:
    goto exit_label;
    break;
  }
  goto start_label;

exit_label:
  return 0;
}

}
