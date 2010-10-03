/*******************************************************************************
 * Copyright (c) 2010-04-14 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include <btree/btree.h>
#include <callback/callback.h>

#include "../gen_utility.h"
#include "../gen_btree.h"
#include "../gen_switch.h"
#include <cb_gen/cb_gen.h>

using namespace cb;

namespace cb_gen {

int sequence_memory_needed( Node* n )
{
  Node* c = get_first_child( n );
  if( !c )
    return 0;

  int mem = sizeof( unsigned int ) * 2;
  int max = 0;
  while( c )
  {
    int t = memory_needed( c );
    if( t > max )
      max = t;
    c = c->m_Next;
  }
  return mem + max;
}

int sequence_setup( Node* n )
{
  Node* c = get_first_child( n );
  while( c )
  {
    if( int r = setup( c ) )
      return r;
    c = c->m_Next;
  }
  return 0;
}

void gen_sequence_con( Function* f, Node* n, uint prio )
{
  //Grab a register
  uchar r = alloc_register( f, prio );
  //Init r to uninitialized
  set_registry( f->m_I, r, 0xffffffff );
  //Store it in memory on first offset
  store_with_offset( f->m_I, ems, 0, r );
  //Store it in memory on second offset
  store_with_offset( f->m_I, ems, 1, r );
  //Free the register
  free_register( f, r );
}

void gen_sequence_exe( Function* f, Node* n, uint prio )
{

}

void gen_sequence_des( Function* f, Node* n, uint prio )
{

}

}
