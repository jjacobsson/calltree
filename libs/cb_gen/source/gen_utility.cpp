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

#define ADD( MACRO_I, MACRO_A1, MACRO_A2, MACRO_A3 ) \
  { \
    Instruction inst; \
    inst.i  = MACRO_I;  \
    inst.a1 = MACRO_A1; \
    inst.a2 = MACRO_A2; \
    inst.a3 = MACRO_A3; \
    i.push_back( inst ); \
  } \


void load_with_offset( InstList& i, uchar fr, uchar tr, uint index )
{
  const uint suint = sizeof( uint );
  uint bo = index * suint;
  if( index <= 0xff )
  {
    ADD( iload, tr, fr, (uchar)index );
  }
  else if( bo <= 0xffff )
  {
    ADD( isetl, tr, (uchar)((bo&0x0000ff00)>>8), (uchar)((bo&0x000000ff)) );
    ADD( iadd,  tr, tr, fr );
    ADD( iload, tr, tr, 0 );
  }
  else if( bo <= (0xffff + (suint*0xff)) )
  {
    uint r = (bo - 0xffff) / suint;
    ADD( isetl, tr, 0xff, 0xff );
    ADD( iadd,  tr, tr, fr );
    ADD( iload, tr, tr, (uchar)(r&0x000000ff) );
  }
  else
  {
    ADD( iseth, tr, (uchar)((bo&0xff000000)>>24), (uchar)((bo&0x00ff0000)>>16) );
    ADD( iorl,  tr, (uchar)((bo&0x0000ff00)>>8), (uchar)(bo&0x000000ff) );
    ADD( iadd,  tr, tr, fr );
    ADD( iload, tr, tr, 0 );
  }
}

}
