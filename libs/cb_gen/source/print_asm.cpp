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
  "r0", /* General purpose", persisting */
  "r1", /* General purpose", persisting */
  "r2", /* General purpose", persisting */
  "r3", /* General purpose", persisting */
  "r4", /* General purpose", volatile */
  "r5", /* General purpose", volatile */
  "r6", /* General purpose", volatile */
  "r7", /* General purpose", volatile */
  "rr", /* Return value register */
  "fc", /* Frame counter */
  "ic", /* Instruction counter */
  "ip", /* Instruction pointer */
  "sp", /* Stack pointer */
  "fs", /* Call stack pointer */
  "ds", /* Data section */
  "ms", /* Memory section */
  "ft", /* Function Lookup Table */
  "jt", /* Jump target lookup table */
};

int print_inst( char* buff, cb::Instruction inst, uint line )
{
  int r = sprintf( buff, "0x%08x  %-6s", line, g_InstNames[inst.i] );

  switch( inst.i )
  {
  case inop:
    r += sprintf( buff + r, "0x%02x 0x%02x 0x%02x", inst.a1, inst.a2, inst.a3 );
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
    r += sprintf( buff + r, "%s" );
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
  case istore:
    r += sprintf( buff + r, "%s,%s(0x%02x)", g_RegNames[inst.a1],g_RegNames[inst.a2], inst.a3 );
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
  num = sprintf( tb, "%-12s%s\n", "Line", "Instruction" );
  afp( tb, num );

  uint i = 0;
  InstList::iterator it, it_e( p->m_I.end() );
  for( it = p->m_I.begin(); it != it_e; ++it, ++i )
  {
    num = print_inst( tb, *it, i );
    afp( tb, num );
  }
}

}
