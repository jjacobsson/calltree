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
  "nop",          /* 0 No-Op */
  "call",         /* 1 */
  "ret",          /* 2 */
  "ccall",        /* 3 */
  "dcall",        /* 4 */
  "br",           /* 5 */
  "bre",          /* 6 */
  "brne",         /* 7 */
  "mov",          /* 8 */
  "add",          /* 9 */
  "sub",          /* 10 */
  "mul",          /* 11 */
  "div",          /* 12 */
  "addi",         /* 13 */
  "subi",         /* 14 */
  "muli",         /* 15 */
  "divi",         /* 16 */
  "ld",           /* 17 */
  "st",           /* 18 */
  "push",         /* 19 */
  "pop",          /* 20 */
  "shli",         /* 21 */
  "shri",         /* 22 */
  "and",          /* 23 */
  "or",           /* 24 */
  "xor",          /* 25 */
  "slszi",        /* 26 */
  "shszi",        /* 27 */
  "andl",         /* 28 */
  "andh",         /* 29 */
  "orl",          /* 30 */
  "orh",          /* 31 */
  "xorl",         /* 32 */
  "xorh",         /* 33 */
  "exit"          /* 34 */
};

const char* const g_RegNames[cb::reg_count] =
{
  "r0",   /* 00 General purpose */
  "r1",   /* 01 General purpose */
  "r2",   /* 02 General purpose */
  "r3",   /* 03 General purpose */
  "r4",   /* 04 General purpose */
  "r5",   /* 05 General purpose */
  "r6",   /* 06 General purpose */
  "r7",   /* 07 General purpose */
  "r8",   /* 08 General purpose */
  "r9",   /* 09 General purpose */
  "r10",  /* 10 General purpose */
  "r11",  /* 11 General purpose */
  "r12",  /* 12 General purpose */
  "r13",  /* 13 General purpose */
  "r14",  /* 14 General purpose */
  "r15",  /* 15 General purpose */
  "r16",  /* 16 General purpose */
  "r17",  /* 17 General purpose */
  "r18",  /* 18 General purpose */
  "r19",  /* 19 General purpose */
  "r20",  /* 20 General purpose */
  "r21",  /* 21 General purpose */
  "rr",   /* 22 return value */
  "fc",   /* 23 Frame counter */
  "ic",   /* 24 Instruction counter */
  "ip",   /* 25 Instruction pointer */
  "sp",   /* 26 Stack pointer */
  "fs",   /* 27 Call stack pointer */
  "ds",   /* 28 Data section */
  "ms",   /* 29 Memory section */
  "ft",   /* 30 Function Lookup Table */
  "jt",   /* 31 Jump target lookup table */
};

int print_inst( char* buff, cb::Instruction inst, uint line )
{
  char op[1024];
  int opc = 0;
  op[0] = 0;
  int r = sprintf( buff, "0x%08x  0x%08x    %-13s", line, *((unsigned int*)(&inst)), g_InstNames[inst.i] );

  switch( inst.i )
  {
  case inop:
    break;
  case icall:
    opc += sprintf( op, "%s", g_RegNames[inst.a1] );
    break;
  case iret:
    break;
  case iccall:
    break;
  case idcall:
    break;
  case ibr:
    opc += sprintf( op, "%s", g_RegNames[inst.a1] );
    break;
  case ibre:
  case ibrne:
    opc += sprintf( op, "%s,%s,%s", g_RegNames[inst.a1],g_RegNames[inst.a2], g_RegNames[inst.a3] );
    break;
  case imov:
    opc += sprintf( op, "%s,%s", g_RegNames[inst.a1], g_RegNames[inst.a2] );
    break;
  case iadd:
  case isub:
  case imul:
  case idiv:
    opc += sprintf( op, "%s,%s,%s", g_RegNames[inst.a1],g_RegNames[inst.a2], g_RegNames[inst.a3] );
    break;
  case iaddi:
  case isubi:
    opc += sprintf( op, "%s,0x%04x", g_RegNames[inst.a1], (((uint)(inst.a2))<<8) | inst.a3 );
    break;
  case ild:
    opc += sprintf( op, "%s,%s(0x%04x)", g_RegNames[inst.a1],g_RegNames[inst.a2], sizeof(unsigned int)*inst.a3 );
    break;
  case ist:
    opc += sprintf( op, "%s(0x%04x),%s", g_RegNames[inst.a1], sizeof(unsigned int)*inst.a2, g_RegNames[inst.a3] );
    break;
  case ipush:
  case ipop:
    opc += sprintf( op, "%s", g_RegNames[inst.a1] );
    break;
  case ishli:
  case ishri:
    opc += sprintf( op, "%s,%s(0x%02x)", g_RegNames[inst.a1],g_RegNames[inst.a2], inst.a3 );
    break;
  case iand:
  case ior:
  case ixor:
    opc += sprintf( op, "%s,%s,%s", g_RegNames[inst.a1],g_RegNames[inst.a2], g_RegNames[inst.a3] );
    break;
  case islszi:
  case ishszi:
  case iandl:
  case iandh:
  case iorl:
  case iorh:
  case ixorl:
  case ixorh:
    opc += sprintf( op, "%s,0x%04x", g_RegNames[inst.a1], (((uint)(inst.a2))<<8) | inst.a3 );
    break;
  case iexit:
    break;
  }
  r += sprintf( buff + r, "%-16s", op );
  return r;
}

int print_labels( Program* p, int line, char* buff )
{
  JumpTargets::iterator jit, jit_e( p->m_J.end() );

  int num = 0;
  int i = 0;
  for( jit = p->m_J.begin(); jit != jit_e; ++jit, i+=sizeof(unsigned int) )
  {
    if( jit->m_A != line )
      continue;
    num += sprintf( buff + num, "jt(0x%04x), ",  i );
  }
  if( num > 0 )
  {
    num -= 2;
    buff[num] = 0;
  }

  num += sprintf( buff + num, "\n" );
  return num;
}

void print_asm( AsmFilePrint afp, Program* p )
{
  char tb[4096];
  int num = 0;
  num = sprintf( tb, "__stub (mem: %d)\n", p->m_Memory );
  afp( tb, num );
  num = sprintf( tb, "%-12s%-14s%-13s%-16s%s\n", "Line" , "Machine Code", "Instruction", "Operands", "Labels" );
  afp( tb, num );

  uint line = 0;


  {
    InstList::iterator it, it_e( p->m_I.end() );
    for( it = p->m_I.begin(); it != it_e; ++it, ++line )
    {
      num = print_inst( tb, *it, line );
      afp( tb, num );
      num = print_labels( p, line, tb );
      afp( tb, num );
    }
  }
  num = sprintf( tb, "\n" );
  afp( tb, num );

  FunctionList::iterator it,it_e( p->m_F.end() );
  for( it = p->m_F.begin(); it != it_e; ++it )
  {
    Function* f = it->m_F;

    num = sprintf( tb, "%s (mem: %d)\n", f->m_T->m_Id.m_Text, f->m_Memory );
    afp( tb, num );
    num = sprintf( tb, "%-12s%-14s%-13s%-16s%s\n", "Line" , "Machine Code", "Instruction", "Operands", "Labels" );
    afp( tb, num );

    {
      InstList::iterator iti, iti_e( f->m_I.end() );
      for( iti = f->m_I.begin(); iti != iti_e; ++iti, ++line )
      {
        num = print_inst( tb, *iti, line );
        afp( tb, num );
        num = print_labels( p, line, tb );
        afp( tb, num );
      }
    }
    num = sprintf( tb, "\n" );
    afp( tb, num );
  }
}

}
