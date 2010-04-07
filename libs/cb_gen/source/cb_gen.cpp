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
#include "gen_btree.h"

#include <other/lookup3.h>

using namespace cb;

namespace cb_gen {

void init( Program* p )
{
  p->m_Memory = 0;
}

void init( Function* f )
{
  f->m_T = 0;
  f->m_P = 0;
  f->m_Memory = 0;
  f->m_Index  = 0;
}

void destroy( Program* p )
{
  FunctionList::iterator it, it_e( p->m_F.end() );
  for( it = p->m_F.begin(); it != it_e; ++it )
  {
    destroy( it->m_F );
    delete it->m_F;
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

void add( Function* f, uchar i, uchar a1, uchar a2, uchar a3 )
{
  Instruction inst;
  inst.i  = i;
  inst.a1 = a1;
  inst.a2 = a2;
  inst.a3 = a3;
  f->m_I.push_back( inst );
}

uint jump_target( Program* p, Function* f )
{
  JumpTarget jt;
  jt.m_Function = f;
  jt.m_Offset   = ~0;
  p->m_J.push_back( jt );
  return p->m_J.size() - 1;
}

void set_offset( Program* p, uint jt, uint offset )
{
  p->m_J[jt].m_Offset = offset;
}

void generate( Function* f )
{
  uint jt_con = jump_target( f->m_P, f );
  uint jt_exe = jump_target( f->m_P, f );
  uint jt_des = jump_target( f->m_P, f );
  uint jt_exi = jump_target( f->m_P, f );

  f->m_I.reserve( 1024 );

  //Load er1 with ACT_CONSTRUCT
  set_registry( f->m_I, er1, ACT_CONSTRUCT );
  //Load er2 with the jump target for the execution code
  load_with_offset( f->m_I, er2, ejt, jt_con );
  //Jump to construction if er0 (arg 1) is ACT_CONSTRUCT
  add( f, ijme, er2, er0, er1 );
  //Load er1 with ACT_EXECUTE
  set_registry( f->m_I, er1, ACT_EXECUTE );
  //Load er2 with the jump target for the execution code
  load_with_offset( f->m_I, er2, ejt, jt_exe );
  //Jump to execution if er0 (arg 1) is ACT_EXECUTE
  add( f, ijme, er2, er0, er1 );
  //Load er1 with ACT_EXECUTE
  set_registry( f->m_I, er1, ACT_DESTRUCT );
  //Load er2 with the jump target for the destruction code
  load_with_offset( f->m_I, er2, ejt, jt_des );
  //Jump to destruction if er0 (arg 1) is ACT_DESTRUCT
  add( f, ijme, er2, er0, er1 );

  //Load er2 with exit label
  load_with_offset( f->m_I, er2, ejt, jt_exi );
  //Jump to exit
  add( f, ijmp, er2, 0, 0 );

  // *** Construction Block *** //

  //Set the offset for the "con" jump
  set_offset( f->m_P, jt_con, f->m_I.size() );
  //Per node jump-target
  uint jt = jump_target( f->m_P, f );
  //Set er2 to the first node's jump target
  set_registry( f->m_I, er2, jt );

  // *** Execute Block *** //
  //Set the offset for the "exe" jump
  set_offset( f->m_P, jt_exe, f->m_I.size() );

  Node* n = f->m_T->m_Root;
  while( n )
  {
    gen_node_con( f, n );
    gen_node_exe( f, n );
    gen_node_des( f, n );
  }

  // *** Destruct Block *** //
  //Set the offset for the "exe" jump
  set_offset( f->m_P, jt_des, f->m_I.size() );


  //Set the offset for the "exit" jump
  set_offset( f->m_P, jt_exi, f->m_I.size() );
  //Return
  add( f, iret, 0, 0, 0 );
}

void generate_functions( Program* p )
{
  FunctionList::iterator it,it_e( p->m_F.end() );
  for( it = p->m_F.begin(); it != it_e; ++it )
  {
    Function* f = it->m_F;
    generate( f );
  }
}

void generate( BehaviorTreeContext ctx, Program* p )
{
  create_functions( ctx, p );
  generate_functions( p );

  //"Main Entry Function" function index
  uint mef = find_function( p, "main" );
  //"Jump To Execute" jump target
  uint jte = jump_target( p, 0x0 );
  //"Jump To EXit" jump target
  uint jtex = jump_target( p, 0x0 );

  //Set r0 to ACT_CONSTRUCT
  set_registry( p->m_I, er0, ACT_CONSTRUCT );
  //Load the current tree state from memory into r1
  load_with_offset( p->m_I, er1, ems, 0 );
  //Setup the jump target in er2
  load_with_offset( p->m_I, er2, ejt, jte );
  //Jump past call if already constructed
  add( p, ijne, er0, er1, er2 );
  //Call the "main" tree's function
  dressed_call( p->m_I, er20, mef, 4 );

  //Set the jte offset
  set_offset( p, jte, p->m_I.size() );

  //Set er0 to ACT_EXECUTE
  set_registry( p->m_I, er0, ACT_EXECUTE );
  //Store er0 (ACT_EXECUTE) in the state memory
  store_with_offset( p->m_I, er0, ems, 0 );

  //Call the "main" tree's function
  dressed_call( p->m_I, er20, mef, 4 );

  //Setup the jump target in er0
  load_with_offset( p->m_I, er0, ejt, jtex );
  //Setup er1 with the return value we want to check
  set_registry( p->m_I, er1, E_NODE_WORKING );
  //Jump past destruction if the tree is "non-finished"
  add( p, ijme, er0, er1, err );

  //Setup er0 to tell the tree to destruct
  set_registry( p->m_I, er0, ACT_DESTRUCT );
  //Call the "main" tree's function
  dressed_call( p->m_I, er20, mef, 4 );

  //Setup er0 to ACT_CONSTRUCT
  set_registry( p->m_I, er0, ACT_CONSTRUCT );
  //Store er0 in state memory
  store_with_offset( p->m_I, er0, ems, 0 );

  //Set the jtex offset
  set_offset( p, jtex, p->m_I.size() );

  //Exit
  add( p, iexit, 0, 0, 0 );
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

