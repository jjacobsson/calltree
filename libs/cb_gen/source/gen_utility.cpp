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

#include "gen_utility.h"

namespace cb_gen {

using namespace cb;

void add( InstList& il, uchar i, uchar a1, uchar a2, uchar a3 )
{
  Instruction inst;
  inst.i  = i;
  inst.a1 = a1;
  inst.a2 = a2;
  inst.a3 = a3;
  il.push_back( inst );
}

void load_with_offset( InstList& il, uchar to, uchar from, uint index )
{
  const uint suint = sizeof( uint );
  uint bo = index * suint;
  if( index <= 0xff )
  {
    add( il, iload, to, from, (uchar)index );
  }
  else if( bo <= 0xffff )
  {
    add( il, isetl, to, (uchar)((bo&0x0000ff00)>>8), (uchar)((bo&0x000000ff)) );
    add( il, iadd,  to, to, from );
    add( il, iload, to, to, 0 );
  }
  else if( bo <= (0xffff + (suint*0xff)) )
  {
    uint r = (bo - 0xffff) / suint;
    add( il, isetl, to, 0xff, 0xff );
    add( il, iadd,  to, to, from );
    add( il, iload, to, to, (uchar)(r&0x000000ff) );
  }
  else
  {
    add( il, iseth, to, (uchar)((bo&0xff000000)>>24), (uchar)((bo&0x00ff0000)>>16) );
    add( il, iorl,  to, (uchar)((bo&0x0000ff00)>>8), (uchar)(bo&0x000000ff) );
    add( il, iadd,  to, to, from );
    add( il, iload, to, to, 0 );
  }
}

void store_with_offset( InstList& il, uchar to, uchar from, uint index )
{
  const uint suint = sizeof( uint );
  uint bo = index * suint;
  if( index <= 0xff )
  {
    add( il, istore, to, from, (uchar)index );
  }
  else if( bo <= 0xffff )
  {
    add( il, isetl,  to, (uchar)((bo&0x0000ff00)>>8), (uchar)((bo&0x000000ff)) );
    add( il, iadd,   to, to, from );
    add( il, istore, to, to, 0 );
  }
  else if( bo <= (0xffff + (suint*0xff)) )
  {
    uint r = (bo - 0xffff) / suint;
    add( il, isetl,  to, 0xff, 0xff );
    add( il, iadd,   to, to, from );
    add( il, istore, to, to, (uchar)(r&0x000000ff) );
  }
  else
  {
    add( il, iseth,  to, (uchar)((bo&0xff000000)>>24), (uchar)((bo&0x00ff0000)>>16) );
    add( il, iorl,   to, (uchar)((bo&0x0000ff00)>>8), (uchar)(bo&0x000000ff) );
    add( il, iadd,   to, to, from );
    add( il, istore, to, to, 0 );
  }
}

void set_registry( InstList& il, uchar reg, uint value )
{
  if( value <= 0xffff )
  {
    add( il, isetl, reg, (uchar)((value&0x0000ff00)>>8), (uchar)(value&0x000000ff) );
  }
  else
  {
    add( il, iseth, reg, (uchar)((value&0xff000000)>>24), (uchar)((value&0x00ff0000)>>16) );
    add( il, iorl,  reg, (uchar)((value&0x0000ff00)>>8), (uchar)(value&0x000000ff) );
  }
}

void dressed_call( InstList& il, uchar reg, uint func_id, uint mem_offset )
{
  if( mem_offset <= 0xffff )
  {
    //Push ems to the stack
    add( il, ipush, ems, 0, 0 );
    //Increment ems by mem_offset
    add( il, iinc, ems, (uchar)((mem_offset&0x0000ff00)>>8), (uchar)(mem_offset&0x000000ff) );
  }
  else
  {
    //Set the memory offset in reg
    set_registry( il, reg, mem_offset );
    //Push ems to the stack
    add( il, ipush, ems, 0, 0 );
    //Set ems = ems + reg
    add( il, iadd, ems, ems, reg );
  }
  //Set er7 to the function id
  set_registry( il, reg, func_id );
  //Call function
  add( il, icall, reg, 0, 0 );
  //Restore ems
  add( il, ipop, ems, 0, 0 );
}


}
