/*******************************************************************************
 * Copyright (c) 2010-04-07 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include <cb_gen/cb_gen.h>
#include <callback/callback.h>
#include <stdio.h>

namespace cb_gen
{

using namespace cb;

const char* const g_InstNames[cb::inst_count] =
{
  "nop",          /* No-Op */
  "call",         /* push "p+1 to sp and set "p to toc[a1] */
  "ret",          /* pop sp to "p */
  "ccall",        /* */
  "dcall",        /* */
  "jmp",          /* */
  "jme",          /* */
  "jne",          /* */
  "mov",          /* */
  "add",          /* */
  "sub",          /* */
  "mul",          /* */
  "div",          /* */
  "inc",          /* */
  "dec",          /* */
  "load",         /* */
  "store",        /* */
  "push",         /* */
  "pop",          /* */
  "shftl",        /* */
  "shftr",        /* */
  "and",          /* */
  "or",           /* */
  "xor",          /* */
  "setl",         /* */
  "seth",         /* */
  "andl",         /* */
  "andh",         /* */
  "orl",          /* */
  "orh",          /* */
  "xorl",         /* */
  "xorh",         /* */
  "exit",         /* */
};

const char* const g_RegNames[cb::reg_count] =
{
  "r0",
  "r1",
  "r2",
  "r3",
  "r4",
  "r5",
  "r6",
  "r7",
  "r8",
  "r9",
  "r10",
  "r11",
  "r12",
  "r13",
  "r14",
  "r15",
  "r16",
  "r17",
  "r18",
  "r19",
  "r20",
  "rr",
  "fc",
  "ic",
  "ip",
  "sp",
  "fs",
  "ds",
  "ms",
  "ft",
  "jt"
};

int print_inst( char* buff, cb::Instruction inst, uint line )
{
  int r = sprintf( buff, "0x%08x  0x%08x    %-13s", line, *((unsigned int*)(&inst)), g_InstNames[inst.i] );

  switch( inst.i )
  {
  case inop:
    break;
  case icall:
    r += sprintf( buff + r, "%s", g_RegNames[inst.a1] );
    break;
  case iret:
    break;
  case iccall:
    break;
  case idcall:
    break;
  case ijmp:
    r += sprintf( buff + r, "%s", g_RegNames[inst.a1] );
    break;
  case ijme:
  case ijne:
    r += sprintf( buff + r, "%s,%s,%s", g_RegNames[inst.a1],g_RegNames[inst.a2], g_RegNames[inst.a3] );
    break;
  case imov:
    break;
  case iadd:
  case isub:
  case imul:
  case idiv:
    r += sprintf( buff + r, "%s,%s,%s", g_RegNames[inst.a1],g_RegNames[inst.a2], g_RegNames[inst.a3] );
    break;
  case iinc:
  case idec:
    r += sprintf( buff + r, "%s,0x%04x", g_RegNames[inst.a1], (((uint)(inst.a2))<<8) | inst.a3 );
    break;
  case iload:
    r += sprintf( buff + r, "%s,%s(0x%02x)", g_RegNames[inst.a1],g_RegNames[inst.a2], inst.a3 );
    break;
  case istore:
    r += sprintf( buff + r, "%s(0x%02x),%s", g_RegNames[inst.a1], inst.a2, g_RegNames[inst.a3] );
    break;
  case ipush:
  case ipop:
    r += sprintf( buff + r, "%s", g_RegNames[inst.a1] );
    break;
  case ishftl:
  case ishftr:
    r += sprintf( buff + r, "%s,%s(0x%02x)", g_RegNames[inst.a1],g_RegNames[inst.a2], inst.a3 );
    break;
  case iand:
  case ior:
  case ixor:
    r += sprintf( buff + r, "%s,%s,%s", g_RegNames[inst.a1],g_RegNames[inst.a2], g_RegNames[inst.a3] );
    break;
  case isetl:
  case iseth:
  case iandl:
  case iandh:
  case iorl:
  case iorh:
  case ixorl:
  case ixorh:
    r += sprintf( buff + r, "%s,0x%04x", g_RegNames[inst.a1], (((uint)(inst.a2))<<8) | inst.a3 );
    break;
  case iexit:
    break;
  }
  r += sprintf( buff + r, "\n" );
  return r;
}

void print_asm( AsmFilePrint afp, Program* p )
{
  char tb[4096];
  int num = 0;
  num = sprintf( tb, "%-12s%-14s%-13s%s\n", "Line" , "Machine Code", "Instruction", "Operands" );
  afp( tb, num );

  uint line = 0;

  {
    InstList::iterator it, it_e( p->m_I.end() );
    for( it = p->m_I.begin(); it != it_e; ++it, ++line )
    {
      num = print_inst( tb, *it, line );
      afp( tb, num );
    }
  }
  num = sprintf( tb, "\n" );
  afp( tb, num );

  FunctionList::iterator it,it_e( p->m_F.end() );
  for( it = p->m_F.begin(); it != it_e; ++it )
  {
    Function* f = it->m_F;

    num = sprintf( tb, "%s\n", f->m_T->m_Id.m_Text );
    afp( tb, num );
    num = sprintf( tb, "%-12s%-14s%-13s%s\n", "Line" , "Machine Code", "Instruction", "Operands" );
    afp( tb, num );

    {
      InstList::iterator iti, iti_e( f->m_I.end() );
      for( iti = f->m_I.begin(); iti != iti_e; ++iti, ++line )
      {
        num = print_inst( tb, *iti, line );
        afp( tb, num );
      }
    }
    num = sprintf( tb, "\n" );
    afp( tb, num );
  }
}

}
