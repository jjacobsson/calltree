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
#include "node_generation_data.h"

#include <other/lookup3.h>

using namespace cb;

namespace cb_gen
{

void init( Program* p )
{
  p->m_Memory = 0;
}

void init( RegState* r )
{
  r->m_InUse = false;
  r->m_Stack = 0;
}

void init( Function* f )
{
  f->m_T = 0;
  f->m_P = 0;
  f->m_Memory = 0;
  f->m_Index = 0;

  for( int i = 0; i < cb::gen_reg_count; ++i )
    init( &(f->m_Reg[i]) );

  f->m_Reg[er0].m_InUse = true;
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

uint jump_target( Program* p, Function* f )
{
  JumpTarget jt;
  jt.m_F = f;
  jt.m_R = ~0;
  jt.m_A = ~0;
  p->m_J.push_back( jt );
  return p->m_J.size() - 1;
}

void set_offset( Program* p, uint jt, uint offset )
{
  p->m_J[jt].m_R = offset;
}

void generate( Function* f )
{
  uint jt_con = jump_target( f->m_P, f );
  uint jt_exe = jump_target( f->m_P, f );
  uint jt_des = jump_target( f->m_P, f );
  uint jt_exi = jump_target( f->m_P, f );

  f->m_I.reserve( 1024 );

  NodeGenerationNeeds needs;
  init( needs );

  uchar r_arg0    = er0;
  uchar r_br_comp = allocate_register( needs );
  uchar r_br_node = allocate_register( needs );
  uchar r_br_exit = allocate_register( needs );
  uchar r_rr_comp = allocate_register( needs );
  uchar r_ms      = allocate_register( needs );

  //Load r_br_exit with the exit function jump target
  load_with_offset( f->m_I, r_br_exit, ejt, jt_exi );

  add( f, imov, r_ms, ems, 0 );
  add( f, iaddi, ems, 0x00, 0x04 );

  //Load r_br_comp with ACT_CONSTRUCT
  set_registry( f->m_I, r_br_comp, ACT_CONSTRUCT );
  //Load r_br_node with the jump target for the execution code
  load_with_offset( f->m_I, r_br_node, ejt, jt_con );
  //Jump to construction if r_arg0 is ACT_CONSTRUCT
  add( f, ibre, r_br_node, r_arg0, r_br_comp );

  //Load r_br_comp with ACT_EXECUTE
  set_registry( f->m_I, r_br_comp, ACT_EXECUTE );
  //Load r_br_node with the jump target for the execution code
  load_with_offset( f->m_I, r_br_node, ejt, jt_exe );
  //Jump to execution if r_arg0 is ACT_EXECUTE
  add( f, ibre, r_br_node, r_arg0, r_br_comp );

  //Load r_br_comp with ACT_DESTRUCT
  set_registry( f->m_I, r_br_comp, ACT_DESTRUCT );
  //Load r_br_node with the jump target for the destruction code
  load_with_offset( f->m_I, r_br_node, ejt, jt_des );
  //Jump to destruction if r_arg0 is ACT_DESTRUCT
  add( f, ibre, r_br_node, r_arg0, r_br_comp );

  //Load r_br_node with exit label
  load_with_offset( f->m_I, r_br_node, ejt, jt_exi );
  //Jump to exit
  add( f, ibr, r_br_node, 0, 0 );

  //Only here to "break" the asm so it's more clear
  add( f, inop, 0, 0, 0 );

  // *** Construction Block *** //

  //Set the offset for the "con" jump
  set_offset( f->m_P, jt_con, f->m_I.size() );
  //Set r_br_node to "uninitialized"
  set_registry( f->m_I, r_br_node, 0xffffffff );
  //"clear" entry point memory
  store_with_offset( f->m_I, r_ms, 0, r_br_node );
  //Get the exit jump
  load_with_offset( f->m_I, er1, ejt, jt_exi );
  //Jump out
  add( f, ibr, er1, 0, 0 );
  //Only here to "break" the asm so it's more clear
  add( f, inop, 0, 0, 0 );

  // *** Execute Block *** //
  //Set the offset for the "exe" jump
  set_offset( f->m_P, jt_exe, f->m_I.size() );
  //Set comparison registry to "success"
  set_registry( f->m_I, r_rr_comp, E_NODE_SUCCESS );
  //Load jump target from memory
  load_with_offset( f->m_I, r_ms, r_br_node, 0 );
  //Set r_br_comp to "uninitialized"
  set_registry( f->m_I, r_br_comp, 0xffffffff );
  //Jump to the remembered node if it is initialized
  add( f, ibrne, r_br_node, r_br_node, r_br_comp );

  Node* n = f->m_T->m_Root;
  while( n )
  {
    //Generate construction code for the node
    gen_node_con( f, n, 1 );
    //Get a jump target for node entry
    uint jt = jump_target( f->m_P, f );
    //Set er1 to jump target
    set_registry( f->m_I, r_br_node, jt );
    //Store r_ms in memory for re-entry
    store_with_offset( f->m_I, r_ms, 0, r_br_node );
    //Set the offset for the jump target
    set_offset( f->m_P, jt, f->m_I.size() );
    //Generate the node's execution code
    gen_node_exe( f, n, 1 );
    //Jump out of the function if "working" or "fail"
    add( f, ibrne, r_br_exit, err, r_rr_comp );
    //Destroy the child node
    gen_node_des( f, n, 1 );
    //And advance to the next node
    n = n->m_Next;
  }

  //Only here to "break" the asm so it's more clear
  add( f, inop, 0, 0, 0 );

  // *** Destruct Block *** //
  //Set the offset for the "des" jump
  set_offset( f->m_P, jt_des, f->m_I.size() );
  //Set r_br_node to "uninitialized"
  set_registry( f->m_I, r_br_node, 0xffffffff );
  //"clear" entry point memory
  store_with_offset( f->m_I, r_ms, 0, r_br_node );

  //Only here to "break" the asm so it's more clear
  add( f, inop, 0, 0, 0 );

  //Set the offset for the "exit" jump
  set_offset( f->m_P, jt_exi, f->m_I.size() );
  //Restore the ms register
  add( f, imov, ems, r_ms, 0 );
  //Return
  add( f, iret, 0, 0, 0 );
}

void generate_functions( Program* p )
{
  FunctionList::iterator it, it_e( p->m_F.end() );
  for( it = p->m_F.begin(); it != it_e; ++it )
  {
    Function* f = it->m_F;
    generate( f );
  }
}

void count_memory( Function* f )
{
  int mem = sizeof( unsigned int ) * 2;
  f->m_Memory = mem + memory_needed( f->m_T );
}

void count_memory( Program* p )
{
  FunctionList::iterator it, it_e( p->m_F.end() );
  for( it = p->m_F.begin(); it != it_e; ++it )
  {
    Function* f = it->m_F;
    count_memory( f );
  }
}

void generate( BehaviorTreeContext ctx, Program* p )
{
  create_functions( ctx, p );
  generate_functions( p );
  count_memory( p );

  //"Main Entry Function" function index
  uint mef = find_function( p, "main" );
  //Set the memory need's for this program
  p->m_Memory = p->m_F[mef].m_F->m_Memory + 4;
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
  add( p, ibrne, er2, er1, er0 );
  //Call the "main" tree's function
  dressed_call( p->m_I, er20, mef, 4 );

  //Set the jte offset
  set_offset( p, jte, p->m_I.size() );

  //Set er0 to ACT_EXECUTE
  set_registry( p->m_I, er0, ACT_EXECUTE );
  //Store er0 (ACT_EXECUTE) in the state memory
  store_with_offset( p->m_I, er0, 0, ems );

  //Call the "main" tree's function
  dressed_call( p->m_I, er20, mef, 4 );

  //Setup the jump target in er0
  load_with_offset( p->m_I, er0, ejt, jtex );
  //Setup er1 with the return value we want to check
  set_registry( p->m_I, er1, E_NODE_WORKING );
  //Jump past destruction if the tree is "non-finished"
  add( p, ibre, er0, er1, err );

  //Setup er0 to tell the tree to destruct
  set_registry( p->m_I, er0, ACT_DESTRUCT );
  //Call the "main" tree's function
  dressed_call( p->m_I, er20, mef, 4 );

  //Setup er0 to ACT_CONSTRUCT
  set_registry( p->m_I, er0, ACT_CONSTRUCT );
  //Store er0 in state memory
  store_with_offset( p->m_I, er0, 0, ems );

  //Set the jtex offset
  set_offset( p, jtex, p->m_I.size() );

  //Exit
  add( p, iexit, 0, 0, 0 );

  translate_jump_labels( p );
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

void translate_jump_labels( Program* p )
{
  JumpTargets::iterator jit, jit_e( p->m_J.end() );
  for( jit = p->m_J.begin(); jit != jit_e; ++jit )
  {
    if( jit->m_F == 0x0 )
      jit->m_A = jit->m_R;
  }

  unsigned int offset = p->m_I.size();
  FunctionList::iterator fit, fit_e( p->m_F.end() );
  for( fit = p->m_F.begin(); fit != fit_e; ++fit )
  {
    for( jit = p->m_J.begin(); jit != jit_e; ++jit )
    {
      if( jit->m_F == fit->m_F )
        jit->m_A = offset + jit->m_R;
    }
    offset += fit->m_F->m_I.size();
  }
}

}

