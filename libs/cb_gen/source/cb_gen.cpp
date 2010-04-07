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
#include "gen_utility.h"

#include <other/lookup3.h>

using namespace cb;

namespace cb_gen {

void init( Program* p )
{
  p->m_Memory = 0;
}

void init( Function* f )
{
  f->m_Memory = 0;
  f->m_Index  = 0;
}

void destroy( Program* p )
{
  FunctionList::iterator it, it_e( p->m_F.end() );
  for( it = p->m_F.begin(); it != it_e; ++it )
  {
    destroy( it->m_Function );
    delete it->m_Function;
  }
}

void destroy( Function* f )
{

}

void add( Program* p, uchar i, uchar a1, uchar a2, uchar a3 )
{
  Instruction inst;
  inst.i  = i;
  inst.a1 = a1;
  inst.a2 = a2;
  inst.a3 = a3;
  p->m_I.push_back( inst );
}

void generate( BehaviorTreeContext ctx, Program* p )
{
  //"Main Entry Function" function index
  uint mef = find_function( p, "main" );
  //FIXME "Jump To Execute" jump target
  uint jte = 0;
  //FIXME "Jump To EXit" jump target
  uint jtex = 0;

  //Set r0 to ACT_CONSTRUCT
  set_registry( p->m_I, er0, ACT_CONSTRUCT );
  //Load the current tree state from memory into r1
  load_with_offset( p->m_I, er1, ems, 0 );
  //Setup the jump target in er2
  load_with_offset( p->m_I, er2, ejt, jte );
  //Jump past call if already constructed
  add( p, ijne, er0, er1, er2 );
  //Call the "main" tree's function
  dressed_call( p->m_I, mef, 4 );


  //FIXME
  //Set the jte here!

  //Set er0 to ACT_EXECUTE
  set_registry( p->m_I, er0, ACT_EXECUTE );
  //Store er0 (ACT_EXECUTE) in the state memory
  store_with_offset( p->m_I, er0, ems, 0 );

  //Call the "main" tree's function
  dressed_call( p->m_I, mef, 4 );

  //Setup the jump target in er0
  load_with_offset( p->m_I, er0, ejt, jtex );
  //Setup er1 with the return value we want to check
  set_registry( p->m_I, er1, E_NODE_WORKING );
  //Jump past destruction if the tree is "non-finished"
  add( p, ijme, er0, er1, err );

  //Setup er0 to tell the tree to destruct
  set_registry( p->m_I, er0, ACT_DESTRUCT );
  //Call the "main" tree's function
  dressed_call( p->m_I, mef, 4 );

  //Setup er0 to ACT_CONSTRUCT
  set_registry( p->m_I, er0, ACT_CONSTRUCT );
  //Store er0 in state memory
  store_with_offset( p->m_I, er0, ems, 0 );

  //FIXME
  //Set the jtex target here

  //Exit
  add( p, iexit, 0, 0, 0 );
}

uint generate( BehaviorTree* t, Program* p )
{
  return ~0;
}

uint find_function( Program* p, const char* name )
{
  if( !name )
    return ~0;
  return find_function( p, hashlittle( name ) );
}

uint find_function( Program* p, hash_t h )
{
  uint i = 0;
  FunctionList::iterator it, it_e( p->m_F.end() );
  for( it = p->m_F.begin(); it != it_e; ++it, ++i )
  {
    if( it->m_Id == h )
      break;
  }
  if( i == p->m_F.size() )
    i = ~0;

  return i;
}

}

