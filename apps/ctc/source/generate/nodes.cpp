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

#include "nodes.h"
#include "program.h"

#include <btree/btree_data.h>
#include <btree/btree_func.h>
#include <callback/callback.h>
#include <other/lookup3.h>

#include <vector>

using namespace callback;

typedef std::vector<int> IntVector;

struct VariableGenerateData
{
  int m_bssStart;
  IntVector m_Data;
};

int store_variables_in_data_section( VariableGenerateData* vd, Variable* vars,
  Variable* dec, Program* p );
int generate_variable_instructions( VariableGenerateData* vd, Variable* vars,
  Program* p );
int setup_variable_registry( VariableGenerateData* vd, Variable* vars,
  Program* p );

int setup_gen( Node* n, Program* p )
{
  int r = 0;
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_SEQUENCE:
    r = gen_setup_sequence( n, p );
    break;
  case E_GRIST_SELECTOR:
    r = gen_setup_selector( n, p );
    break;
  case E_GRIST_PARALLEL:
    r = gen_setup_parallel( n, p );
    break;
  case E_GRIST_DYN_SELECTOR:
    r = gen_setup_dynselector( n, p );
    break;
  case E_GRIST_SUCCEED:
    r = gen_setup_succeed( n, p );
    break;
  case E_GRIST_FAIL:
    r = gen_setup_fail( n, p );
    break;
  case E_GRIST_WORK:
    r = gen_setup_work( n, p );
    break;
  case E_GRIST_DECORATOR:
    r = gen_setup_decorator( n, p );
    break;
  case E_GRIST_ACTION:
    r = gen_setup_action( n, p );
    break;
  default:
    r = -1;
    break;
  }
  return r;
}

int teardown_gen( Node* n, Program* p )
{
  int r = 0;
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_SEQUENCE:
    r = gen_teardown_sequence( n, p );
    break;
  case E_GRIST_SELECTOR:
    r = gen_teardown_selector( n, p );
    break;
  case E_GRIST_PARALLEL:
    r = gen_teardown_parallel( n, p );
    break;
  case E_GRIST_DYN_SELECTOR:
    r = gen_teardown_dynselector( n, p );
    break;
  case E_GRIST_SUCCEED:
    r = gen_teardown_succeed( n, p );
    break;
  case E_GRIST_FAIL:
    r = gen_teardown_fail( n, p );
    break;
  case E_GRIST_WORK:
    r = gen_teardown_work( n, p );
    break;
  case E_GRIST_DECORATOR:
    r = gen_teardown_decorator( n, p );
    break;
  case E_GRIST_ACTION:
    r = gen_teardown_action( n, p );
    break;
  default:
    r = -1;
    break;
  }
  return r;
}

int gen_con( Node* n, Program* p )
{
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_SEQUENCE:
    return gen_con_sequence( n, p );
    break;
  case E_GRIST_SELECTOR:
    return gen_con_selector( n, p );
    break;
  case E_GRIST_PARALLEL:
    return gen_con_parallel( n, p );
    break;
  case E_GRIST_DYN_SELECTOR:
    return gen_con_dynselector( n, p );
    break;
  case E_GRIST_SUCCEED:
    return gen_con_succeed( n, p );
    break;
  case E_GRIST_FAIL:
    return gen_con_fail( n, p );
    break;
  case E_GRIST_WORK:
    return gen_con_work( n, p );
    break;
  case E_GRIST_DECORATOR:
    return gen_con_decorator( n, p );
    break;
  case E_GRIST_ACTION:
    return gen_con_action( n, p );
    break;
  }
  return -1;
}

int gen_exe( Node* n, Program* p )
{
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_SEQUENCE:
    return gen_exe_sequence( n, p );
    break;
  case E_GRIST_SELECTOR:
    return gen_exe_selector( n, p );
    break;
  case E_GRIST_PARALLEL:
    return gen_exe_parallel( n, p );
    break;
  case E_GRIST_DYN_SELECTOR:
    return gen_exe_dynselector( n, p );
    break;
  case E_GRIST_SUCCEED:
    return gen_exe_succeed( n, p );
    break;
  case E_GRIST_FAIL:
    return gen_exe_fail( n, p );
    break;
  case E_GRIST_WORK:
    return gen_exe_work( n, p );
    break;
  case E_GRIST_DECORATOR:
    return gen_exe_decorator( n, p );
    break;
  case E_GRIST_ACTION:
    return gen_exe_action( n, p );
    break;
  }
  return -1;
}

int gen_des( Node* n, Program* p )
{
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_SEQUENCE:
    return gen_des_sequence( n, p );
    break;
  case E_GRIST_SELECTOR:
    return gen_des_selector( n, p );
    break;
  case E_GRIST_PARALLEL:
    return gen_des_parallel( n, p );
    break;
  case E_GRIST_DYN_SELECTOR:
    return gen_des_dynselector( n, p );
    break;
  case E_GRIST_SUCCEED:
    return gen_des_succeed( n, p );
    break;
  case E_GRIST_FAIL:
    return gen_des_fail( n, p );
    break;
  case E_GRIST_WORK:
    return gen_des_work( n, p );
    break;
  case E_GRIST_DECORATOR:
    return gen_des_decorator( n, p );
    break;
  case E_GRIST_ACTION:
    return gen_des_action( n, p );
    break;
  }
  return -1;
}

/*
 *
 * Sequence
 *
 */

struct SequenceNodeData
{
  int m_bss_JumpBackTarget;
  int m_bss_ReEntry;
};

int gen_setup_sequence( Node* n, Program* p )
{
  //Alloc space needed for code generation
  SequenceNodeData* nd = new SequenceNodeData;
  //Store needed generation data in the node's UserData pointer
  n->m_UserData = nd;

  //Alloc storage area for function call stack.
  nd->m_bss_JumpBackTarget = p->m_B.Push( sizeof(int), 4 );
  //Alloc storage area for re-entry instruction
  nd->m_bss_ReEntry = p->m_B.Push( sizeof(int), 4 );

  int r = 0;
  Node* c = GetFirstChild( n );
  while( c && r == 0 )
  {
    p->m_B.PushScope();
    r = setup_gen( c, p );
    p->m_B.PopScope();
    c = c->m_Next;
  }
  return r;
}

int gen_teardown_sequence( Node* n, Program* p )
{
  //Free the space used when generating code.
  delete ((SequenceNodeData*)n->m_UserData);
  n->m_UserData = 0x0;

  int r = 0;
  Node* c = GetFirstChild( n );
  while( c && r == 0 )
  {
    r = teardown_gen( c, p );
    c = c->m_Next;
  }
  return r;
}

int gen_con_sequence( Node* n, Program* p )
{
  SequenceNodeData* nd = (SequenceNodeData*)n->m_UserData;
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_CONSTRUCT );

  //Set jump-back pointer value to uninitialized
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_JumpBackTarget, 0xffffffff, 0 );

  //Set re-entry pointer value to uninitialized
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_ReEntry, 0xffffffff, 0 );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_CONSTRUCT );

  return 0;
}

int gen_exe_sequence( Node* n, Program* p )
{
  SequenceNodeData* nd = (SequenceNodeData*)n->m_UserData;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_EXECUTE );

  IntVector exit_working;
  IntVector exit_fail;
  IntVector dest_jumps;

  //Jump to re-entry point if set
  p->m_I.Push( INST_JABB_C_DIFF_B, nd->m_bss_ReEntry, 0xffffffff,
    nd->m_bss_ReEntry );
  int err;
  Node* c = GetFirstChild( n );
  while( c )
  {
    //call child-node construction code
    if( (err = gen_con( c, p )) != 0 )
      return err;

    //store re-entry pointer
    p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_ReEntry, p->m_I.Count() + 1, 0 );

    //call child-node execution code
    if( (err = gen_exe( c, p )) != 0 )
      return err;

    //set the destruction jump pointer (we re-use the bss for jump-back-target for this)
    dest_jumps.push_back( p->m_I.Count() );
    p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_JumpBackTarget, 0xffffffff, 0 );

    //"exit if working" jump.
    exit_working.push_back( p->m_I.Count() );
    p->m_I.Push( INST_JABC_R_EQUA_C, 0xffffffff, E_NODE_WORKING, 0 );

    //Jump to destruction code and set the jump-back at the same time
    dest_jumps.push_back( p->m_I.Count() );
    p->m_I.Push( INST_JABC_S_C_IN_B, 0xffffffff, nd->m_bss_JumpBackTarget,
      p->m_I.Count() + 1 );

    //exit this node if return value is non-success
    exit_fail.push_back( p->m_I.Count() );
    p->m_I.Push( INST_JABC_R_DIFF_C, 0xffffffff, E_NODE_SUCCESS, 0 );

    //Iterate
    c = c->m_Next;
  }

  //Success! Jump past all this destruction stuff
  int jump_to_exit_success = p->m_I.Count();
  p->m_I.Push( INST_JABC_CONSTANT, 0xffffffff, 0, 0 );

  //Here we generate the destruction code for all child-nodes.
  int i = 0;
  c = GetFirstChild( n );
  while( c )
  {
    //Patch store destruction code pointer instruction
    p->m_I.SetA2( dest_jumps[0 + i * 2], p->m_I.Count() );
    //Patch jump to destruction instruction
    p->m_I.SetA1( dest_jumps[1 + i * 2], p->m_I.Count() );
    //call child-node destruction code
    if( (err = gen_des( c, p )) != 0 )
      return err;
    //Jump back to calling code and "de-initialize the jump-back-target at the same time
    p->m_I.Push( INST_JABB_S_C_IN_B, nd->m_bss_JumpBackTarget,
      nd->m_bss_JumpBackTarget, 0xffffffff );

    //Iterate
    c = c->m_Next;
    ++i;
  }

  //Set return value to node fail
  int exit_fail_point = p->m_I.Count();
  p->m_I.Push( INST__STORE_C_IN_R, E_NODE_FAIL, 0, 0 );

  //Patch the "jump to exit success"
  p->m_I.SetA1( jump_to_exit_success, p->m_I.Count() );

  //clear re-entry pointer
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_ReEntry, 0xffffffff, 0 );

  //Patch jump instruction targets for fail.
  int s;
  s = exit_fail.size();
  for( int i = 0; i < s; ++i )
    p->m_I.SetA1( exit_fail[i], exit_fail_point );

  //Patch jump instruction targets for working.
  int exit_working_point = p->m_I.Count();
  s = exit_working.size();
  for( int i = 0; i < s; ++i )
    p->m_I.SetA1( exit_working[i], exit_working_point );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_EXECUTE );
  return 0;
}

int gen_des_sequence( Node* n, Program* p )
{
  SequenceNodeData* nd = (SequenceNodeData*)n->m_UserData;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_DESTRUCT );

  //Jump past destruction code if m_bss_JumpBackTarget is uninitialized
  p->m_I.Push( INST_JABC_C_EQUA_B, p->m_I.Count() + 2, 0xffffffff,
    nd->m_bss_JumpBackTarget );
  //Jump to destruction code and set the jump-back at the same time
  p->m_I.Push( INST_JABB_S_C_IN_B, nd->m_bss_JumpBackTarget,
    nd->m_bss_JumpBackTarget, p->m_I.Count() + 1 );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_DESTRUCT );

  return 0;
}

/*
 *
 * Selector
 *
 */

struct SelectorNodeData
{
  int m_bss_JumpBackTarget;
  int m_bss_ReEntry;
};

int gen_setup_selector( Node* n, Program* p )
{
  //Alloc space needed for code generation
  SelectorNodeData* nd = new SelectorNodeData;

  //Store needed generation data in the node's UserData pointer
  n->m_UserData = nd;

  //Alloc storage area for function call stack.
  nd->m_bss_JumpBackTarget = p->m_B.Push( sizeof(int), 4 );

  //Alloc storage area for re-entry instruction
  nd->m_bss_ReEntry = p->m_B.Push( sizeof(int), 4 );

  int r = 0;
  Node* c = GetFirstChild( n );
  while( c && r == 0 )
  {
    p->m_B.PushScope();
    r = setup_gen( c, p );
    p->m_B.PopScope();
    c = c->m_Next;
  }
  return r;
}

int gen_teardown_selector( Node* n, Program* p )
{
  //Free the space used when generating code.
  delete ((SelectorNodeData*)n->m_UserData);
  n->m_UserData = 0x0;

  int r = 0;
  Node* c = GetFirstChild( n );
  while( c && r == 0 )
  {
    r = teardown_gen( c, p );
    c = c->m_Next;
  }
  return r;
}

int gen_con_selector( Node* n, Program* p )
{
  SelectorNodeData* nd = (SelectorNodeData*)n->m_UserData;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_CONSTRUCT );

  //Set re-entry pointer value to uninitialized
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_ReEntry, 0xffffffff, 0 );

  //Set re-entry pointer value to uninitialized
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_JumpBackTarget, 0xffffffff, 0 );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_CONSTRUCT );

  return 0;
}

int gen_exe_selector( Node* n, Program* p )
{
  SelectorNodeData* nd = (SelectorNodeData*)n->m_UserData;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_EXECUTE );

  IntVector exit_working;
  IntVector exit_success;
  IntVector dest_jumps;

  //Jump to re-entry point if set
  p->m_I.Push( INST_JABB_C_DIFF_B, nd->m_bss_ReEntry, 0xffffffff,
    nd->m_bss_ReEntry );

  int err;
  Node* c = GetFirstChild( n );
  while( c )
  {
    //call child-node construction code
    if( (err = gen_con( c, p )) != 0 )
      return err;

    //store re-entry pointer
    p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_ReEntry, p->m_I.Count() + 1, 0 );

    //call child-node execution code
    if( (err = gen_exe( c, p )) != 0 )
      return err;

    //set the destruction jump pointer (we re-use the bss for jump-back-target for this)
    dest_jumps.push_back( p->m_I.Count() );
    p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_JumpBackTarget, 0xffffffff, 0 );

    //"exit if working" jump.
    exit_working.push_back( p->m_I.Count() );
    p->m_I.Push( INST_JABC_R_EQUA_C, 0xffffffff, E_NODE_WORKING, 0 );

    //Jump to destruction code and set the jump-back at the same time
    dest_jumps.push_back( p->m_I.Count() );
    p->m_I.Push( INST_JABC_S_C_IN_B, 0xffffffff, nd->m_bss_JumpBackTarget,
      p->m_I.Count() + 1 );

    //exit this node if return value success
    exit_success.push_back( p->m_I.Count() );
    p->m_I.Push( INST_JABC_R_EQUA_C, 0xffffffff, E_NODE_SUCCESS, 0 );

    //Iterate
    c = c->m_Next;
  }

  //Fail! Jump past all this destruction stuff
  int jump_to_exit_fail = p->m_I.Count();
  p->m_I.Push( INST_JABC_CONSTANT, 0xffffffff, 0, 0 );

  //Here we generate the destruction code for all child-nodes.
  int i = 0;
  c = GetFirstChild( n );
  while( c )
  {
    //Patch store destruction code pointer instruction
    p->m_I.SetA2( dest_jumps[0 + i * 2], p->m_I.Count() );
    //Patch jump to destruction instruction
    p->m_I.SetA1( dest_jumps[1 + i * 2], p->m_I.Count() );
    //call child-node destruction code
    if( (err = gen_des( c, p )) != 0 )
      return err;
    //Jump back to calling code and reset the jump-back-target at the same time
    p->m_I.Push( INST_JABB_S_C_IN_B, nd->m_bss_JumpBackTarget,
      nd->m_bss_JumpBackTarget, 0xffffffff );
    //Iterate
    c = c->m_Next;
    ++i;
  }

  //Set return value to node success
  int exit_success_point = p->m_I.Count();
  p->m_I.Push( INST__STORE_C_IN_R, E_NODE_SUCCESS, 0, 0 );

  //Patch the "jump to exit fail"
  p->m_I.SetA1( jump_to_exit_fail, p->m_I.Count() );

  //clear re-entry pointer
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_ReEntry, 0xffffffff, 0 );

  //Patch jump instruction targets for success.
  int s;
  s = exit_success.size();
  for( int i = 0; i < s; ++i )
    p->m_I.SetA1( exit_success[i], exit_success_point );

  //Patch jump instruction targets for working.
  int exit_working_point = p->m_I.Count();
  s = exit_working.size();
  for( int i = 0; i < s; ++i )
    p->m_I.SetA1( exit_working[i], exit_working_point );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_EXECUTE );

  return 0;
}

int gen_des_selector( Node* n, Program* p )
{
  SelectorNodeData* nd = (SelectorNodeData*)n->m_UserData;
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_DESTRUCT );

  //Jump past destruction code if nd->m_bss_JumpBackTarget is uninitialized
  p->m_I.Push( INST_JABC_C_EQUA_B, p->m_I.Count() + 2, 0xffffffff,
    nd->m_bss_JumpBackTarget );
  //Jump to destruction code and set the jump-back at the same time
  p->m_I.Push( INST_JABB_S_C_IN_B, nd->m_bss_JumpBackTarget,
    nd->m_bss_JumpBackTarget, p->m_I.Count() + 1 );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_DESTRUCT );

  return 0;
}

/*
 *
 * Parallel
 *
 */

struct ParallelNodeData
{
  int m_bss_SuccessCounter;
};

int gen_setup_parallel( Node* n, Program* p )
{
  //Alloc space needed for code generation
  ParallelNodeData* nd = new ParallelNodeData;

  //Store needed generation data in the node's UserData pointer
  n->m_UserData = nd;

  //Alloc storage space for the success counter
  nd->m_bss_SuccessCounter = p->m_B.Push( sizeof(int), 4 );

  int r = 0;
  Node* c = GetFirstChild( n );
  while( c && r == 0 )
  {
    r = setup_gen( c, p );
    c = c->m_Next;
  }
  return r;
}

int gen_teardown_parallel( Node* n, Program* p )
{
  //Free the space used when generating code.
  delete ((ParallelNodeData*)n->m_UserData);
  n->m_UserData = 0x0;

  int r = 0;
  Node* c = GetFirstChild( n );
  while( c && r == 0 )
  {
    r = teardown_gen( c, p );
    c = c->m_Next;
  }
  return r;
}

int gen_con_parallel( Node* n, Program* p )
{
  ParallelNodeData* nd = (ParallelNodeData*)n->m_UserData;
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_CONSTRUCT );

  int err;
  Node* c = GetFirstChild( n );
  while( c )
  {
    //call child-node construction code
    if( (err = gen_con( c, p )) != 0 )
      return err;
    //Iterate
    c = c->m_Next;
  }

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_CONSTRUCT );
  return 0;
}

int gen_exe_parallel( Node* n, Program* p )
{
  ParallelNodeData* nd = (ParallelNodeData*)n->m_UserData;
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_EXECUTE );

  //Set the success counter to 0
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_SuccessCounter, 0, 0 );

  IntVector exit_fail;

  int i = 0, err;
  Node* c = GetFirstChild( n );
  while( c )
  {
    //call child-node execution code
    if( (err = gen_exe( c, p )) != 0 )
      return err;

    //jump out if failed
    exit_fail.push_back( p->m_I.Count() );
    p->m_I.Push( INST_JABC_R_EQUA_C, 0xffffffff, E_NODE_FAIL, 0 );

    //Skip count up if working
    p->m_I.Push( INST_JABC_R_DIFF_C, p->m_I.Count() + 2, E_NODE_SUCCESS, 0 );

    //Count up number of successful child nodes.
    p->m_I.Push( INST__INC_BSSVALUE, nd->m_bss_SuccessCounter, 1, 0 );

    //Iterate
    c = c->m_Next;
    ++i;
  }

  //Set return value to working
  p->m_I.Push( INST__STORE_C_IN_R, E_NODE_WORKING, 0, 0 );
  //Jump out if number any child was non-successfull.
  p->m_I.Push( INST_JABC_C_DIFF_B, p->m_I.Count() + 2, i,
    nd->m_bss_SuccessCounter );
  //Set return value to success (skiped if nd->m_bss_SuccessCounter != m_Children->Size())
  p->m_I.Push( INST__STORE_C_IN_R, E_NODE_SUCCESS, 0, 0 );

  //Patch exit-fail jumps
  int exit_point = p->m_I.Count();
  int s = exit_fail.size();
  for( int i = 0; i < s; ++i )
    p->m_I.SetA1( exit_fail[i], exit_point );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_EXECUTE );
  return 0;
}

int gen_des_parallel( Node* n, Program* p )
{
  ParallelNodeData* nd = (ParallelNodeData*)n->m_UserData;
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_DESTRUCT );

  int err;
  Node* c = GetFirstChild( n );
  while( c )
  {
    //call child-node destruction code
    if( (err = gen_des( c, p )) != 0 )
      return err;
    //Iterate
    c = c->m_Next;
  }

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_DESTRUCT );
  return 0;
}

/*
 *
 * Dynamic Selector
 *
 */

struct DynamicSelectorNodeData
{
  int m_bss_NewBranch;
  int m_bss_OldBranch;
  int m_bss_JumpBackTarget;
  int m_bss_RunningChild;
};

int gen_setup_dynselector( Node* n, Program* p )
{
  //Alloc space needed for code generation
  DynamicSelectorNodeData* nd = new DynamicSelectorNodeData;

  //Store needed generation data in the node's UserData pointer
  n->m_UserData = nd;

  //Alloc storage area in bss
  nd->m_bss_NewBranch = p->m_B.Push( sizeof(int), 4 );
  nd->m_bss_OldBranch = p->m_B.Push( sizeof(int), 4 );
  nd->m_bss_JumpBackTarget = p->m_B.Push( sizeof(int), 4 );
  nd->m_bss_RunningChild = p->m_B.Push( sizeof(int) * CountChildNodes( n ), 4 );

  int r = 0;
  Node* c = GetFirstChild( n );
  while( c && r == 0 )
  {
    r = setup_gen( c, p );
    c = c->m_Next;
  }
  return r;
}

int gen_teardown_dynselector( Node* n, Program* p )
{
  //Free the space used when generating code.
  delete ((DynamicSelectorNodeData*)n->m_UserData);
  n->m_UserData = 0x0;

  int r = 0;
  Node* c = GetFirstChild( n );
  while( c && r == 0 )
  {
    r = teardown_gen( c, p );
    c = c->m_Next;
  }
  return r;
}

int gen_con_dynselector( Node* n, Program* p )
{
  DynamicSelectorNodeData* nd = (DynamicSelectorNodeData*)n->m_UserData;
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_CONSTRUCT );
  int i = 0;
  Node* c = GetFirstChild( n );
  while( c )
  {
    //calculate where in the bss this child's return value should be stored
    int bss_current_child = nd->m_bss_RunningChild + (i * sizeof(int));
    //Set bss return value to undefined for all child nodes.
    p->m_I.Push( INST__STORE_C_IN_B, bss_current_child, E_NODE_UNDEFINED, 0 );
    //Iterate
    c = c->m_Next;
    ++i;
  }

  //Set bss values to undefined
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_NewBranch, 0xffffffff, 0 );
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_OldBranch, 0xffffffff, 0 );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_CONSTRUCT );
  return 0;
}

int gen_exe_dynselector( Node* n, Program* p )
{
  DynamicSelectorNodeData* nd = (DynamicSelectorNodeData*)n->m_UserData;
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_EXECUTE );

  IntVector exit_jumps, true_exit_jumps, cons_jumps, exec_jumps, dest_jumps;
  int i = 0;
  Node* c = GetFirstChild( n );
  while( c )
  {
    //calculate where in the bss this child's return value should be stored
    int bss_current_child = nd->m_bss_RunningChild + (i * sizeof(int));

    //Set jump back after construction target.
    p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_JumpBackTarget, p->m_I.Count()
        + 2, 0 );
    //Jump to construction code *if* this child is NOT working.
    cons_jumps.push_back( p->m_I.Count() );
    p->m_I.Push( INST_JABC_C_DIFF_B, 0xffffffff, E_NODE_WORKING,
      bss_current_child );

    //Set this as the "new" executing branch.
    dest_jumps.push_back( p->m_I.Count() );
    p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_NewBranch, 0xffffffff, 0 );

    //Jump to execution code and set the jump-back at the same time
    exec_jumps.push_back( p->m_I.Count() );
    p->m_I.Push( INST_JABC_S_C_IN_B, 0xffffffff, nd->m_bss_JumpBackTarget,
      p->m_I.Count() + 1 );

    //exit if working
    exit_jumps.push_back( p->m_I.Count() );
    p->m_I.Push( INST_JABC_R_EQUA_C, 0xffffffff, E_NODE_WORKING, 0 );

    //Jump to destruction code and set the jump-back at the same time
    dest_jumps.push_back( p->m_I.Count() );
    p->m_I.Push( INST_JABC_S_C_IN_B, 0xffffffff, nd->m_bss_JumpBackTarget,
      p->m_I.Count() + 1 );

    //exit if success
    exit_jumps.push_back( p->m_I.Count() );
    p->m_I.Push( INST_JABC_R_EQUA_C, 0xffffffff, E_NODE_SUCCESS, 0 );

    //Iterate
    c = c->m_Next;
    ++i;
  }

  // Truly jump to exit instruction... this happens when all nodes fail
  true_exit_jumps.push_back( p->m_I.Count() );
  p->m_I.Push( INST_JABC_CONSTANT, 0xffffffff, 0, 0 );

  int err;
  i = 0;
  c = GetFirstChild( n );
  while( c )
  {
    //calculate where in the bss this child's return value should be stored
    int bss_current_child = nd->m_bss_RunningChild + (i * sizeof(int));

    //Patch jump to construction instruction
    p->m_I.SetA1( cons_jumps[i], p->m_I.Count() );
    //call child-node construction code
    if( (err = gen_con( c, p )) != 0 )
      return err;
    //Jump back to calling code
    p->m_I.Push( INST_JABB_BSSVALUE, nd->m_bss_JumpBackTarget, 0, 0 );

    //Patch jump to execution instruction
    p->m_I.SetA1( exec_jumps[i], p->m_I.Count() );
    //call child-node execution code
    if( (err = gen_exe( c, p )) != 0 )
      return err;
    //Store child's return value
    p->m_I.Push( INST__STORE_R_IN_B, bss_current_child, 0, 0 );
    //Jump back to calling code
    p->m_I.Push( INST_JABB_BSSVALUE, nd->m_bss_JumpBackTarget, 0, 0 );

    //Patch store destruction code pointer instruction
    p->m_I.SetA2( dest_jumps[0 + i * 2], p->m_I.Count() );
    //Patch jump to destruction instruction
    p->m_I.SetA1( dest_jumps[1 + i * 2], p->m_I.Count() );
    //call child-node destruction code
    if( (err = gen_des( c, p )) != 0 )
      return err;
    //Set childs stored return value to undefined
    p->m_I.Push( INST__STORE_C_IN_B, bss_current_child, E_NODE_UNDEFINED, 0 );
    //Jump back to calling code
    p->m_I.Push( INST_JABB_BSSVALUE, nd->m_bss_JumpBackTarget, 0, 0 );
    //Iterate
    c = c->m_Next;
    ++i;
  }

  //Time to patch some jumps! wheee!

  //Patch exit jumps
  int exit_point = p->m_I.Count();
  int s = exit_jumps.size();
  for( int i = 0; i < s; ++i )
    p->m_I.SetA1( exit_jumps[i], exit_point );

  //Jump past all this crap if "old branch" is uninitialized
  true_exit_jumps.push_back( p->m_I.Count() );
  p->m_I.Push( INST_JABC_C_EQUA_B, 0xffffffff, 0xffffffff, nd->m_bss_OldBranch );
  //set jump back after destruction target
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_JumpBackTarget,
    p->m_I.Count() + 2, 0 );
  //Jump to destruction code of "old branch" *if* it differs from "new branch"
  p->m_I.Push( INST_JABB_B_DIFF_B, nd->m_bss_OldBranch, nd->m_bss_NewBranch,
    nd->m_bss_OldBranch );
  //Copy "new branch" into "old branch"
  p->m_I.Push( INST__STORE_B_IN_B, nd->m_bss_OldBranch, nd->m_bss_NewBranch, 0 );
  //Set new branch to uninitialized
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_NewBranch, 0xffffffff, 0 );

  //Patch "true" exit jumps
  exit_point = p->m_I.Count();
  s = true_exit_jumps.size();
  for( int i = 0; i < s; ++i )
    p->m_I.SetA1( true_exit_jumps[i], exit_point );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_EXECUTE );
  return 0;
}

int gen_des_dynselector( Node* n, Program* p )
{
  DynamicSelectorNodeData* nd = (DynamicSelectorNodeData*)n->m_UserData;
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_DESTRUCT );

  //Jump past all this crap if "old branch" is uninitialized
  int patch_exit = p->m_I.Count();
  p->m_I.Push( INST_JABC_C_EQUA_B, 0xffffffff, 0xffffffff, nd->m_bss_OldBranch );
  //set jump back after destruction target
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_JumpBackTarget,
    p->m_I.Count() + 2, 0 );
  //Jump to destruction code of "old branch" if it is set to a valid value
  p->m_I.Push( INST_JABB_C_DIFF_B, nd->m_bss_OldBranch, 0xffffffff,
    nd->m_bss_OldBranch );
  //Patch the jump instruction
  p->m_I.SetA1( patch_exit, p->m_I.Count() );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_DESTRUCT );
  return 0;
}

/*
 *
 * Succeed
 *
 */

int gen_setup_succeed( Node* n, Program* p )
{
  return 0;
}

int gen_teardown_succeed( Node* n, Program* p )
{
  return 0;
}

int gen_con_succeed( Node* n, Program* p )
{
  return 0;
}

int gen_exe_succeed( Node* n, Program* p )
{
  p->m_I.Push( INST__STORE_C_IN_R, E_NODE_SUCCESS, 0, 0 );
  return 0;
}

int gen_des_succeed( Node* n, Program* p )
{
  return 0;
}

/*
 *
 * Fail
 *
 */

int gen_setup_fail( Node* n, Program* p )
{
  return 0;
}
int gen_teardown_fail( Node* n, Program* p )
{
  return 0;
}
int gen_con_fail( Node* n, Program* p )
{
  return 0;
}
int gen_exe_fail( Node* n, Program* p )
{
  p->m_I.Push( INST__STORE_C_IN_R, E_NODE_FAIL, 0, 0 );
  return 0;
}
int gen_des_fail( Node* n, Program* p )
{
  return 0;
}

/*
 *
 * Work
 *
 */

int gen_setup_work( Node* n, Program* p )
{
  return 0;
}

int gen_teardown_work( Node* n, Program* p )
{
  return 0;
}

int gen_con_work( Node* n, Program* p )
{
  return 0;
}

int gen_exe_work( Node* n, Program* p )
{
  p->m_I.Push( INST__STORE_C_IN_R, E_NODE_WORKING, 0, 0 );
  return 0;
}

int gen_des_work( Node* n, Program* p )
{
  return 0;
}


/*
 *
 * Decorator
 *
 */

struct DecoratorNodeData
{
  int m_bssPos;
  int m_bssModPos;
  VariableGenerateData m_VD;
};

int gen_setup_decorator( Node* n, Program* p )
{
  Node* c = GetFirstChild( n );
  if( !c )
    return 0;

  //Alloc space needed for code generation
  DecoratorNodeData* nd = new DecoratorNodeData;

  //Store needed generation data in the node's UserData pointer
  n->m_UserData = nd;

  //Get the decorator declaration.
  Decorator* d = n->m_Grist.m_Decorator.m_Decorator;

  Variable* t = FindVariableWithIdHash( d->m_Vars, hashlittle( "bss" ) );
  int bss_need = t ? ValueAsInteger( *t ) : 0;
  t = FindVariableWithIdHash( d->m_Vars, hashlittle( "modify" ) );
  if( t && ValueAsBool( *t ) )
    bss_need += 4;

  if( bss_need > 0 )
  {
    nd->m_bssPos = p->m_B.Push( bss_need, 4 );
    nd->m_bssModPos = (nd->m_bssPos + bss_need) - 4;
  }

  //Store the variable values in the data section.
  store_variables_in_data_section( &nd->m_VD,
    n->m_Grist.m_Decorator.m_Arguments, d->m_Args, p );

  return setup_gen( c, p );
}

int gen_teardown_decorator( Node* n, Program* p )
{
  Node* c = GetFirstChild( n );
  if( !c )
    return 0;

  //Free the space used when generating code.
  delete ((DecoratorNodeData*)n->m_UserData);
  n->m_UserData = 0x0;

  return teardown_gen( c, p );
}

int gen_con_decorator( Node* n, Program* p )
{
  Node* c = GetFirstChild( n );
  if( !c )
    return 0;

  DecoratorNodeData* nd = (DecoratorNodeData*)n->m_UserData;

  Decorator* d = n->m_Grist.m_Decorator.m_Decorator;

  Variable* t = FindVariableWithIdHash( d->m_Vars, hashlittle( "id" ) );
  int fid = t ? ValueAsInteger( *t ) : ~0;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_CONSTRUCT );

  int err;

  // Load bss section with pointers to the data section, for the variables
  err = generate_variable_instructions( &nd->m_VD,
    n->m_Grist.m_Decorator.m_Arguments, p );
  if( err != 0 )
    return err;

  t = FindVariableWithIdHash( d->m_Vars, hashlittle( "construct" ) );
  if( t && ValueAsBool( *t ) )
  {
    //Setup the register for the data pointer
    err = setup_variable_registry( &nd->m_VD,
      n->m_Grist.m_Decorator.m_Arguments, p );
    if( err != 0 )
      return err;
    // Load bss register with bss pointer
    p->m_I.Push( INST_STORE_PB_IN_R, 1, nd->m_bssPos, 0 );
    // Load the register with the correct id
    p->m_I.Push( INST_LOAD_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid
        & 0x0000ffff );
    // Call the decorator construction function
    p->m_I.Push( INST_CALL_CONS_FUN, 0, 1, 2 );
  }

  //Generate child construction code
  if( (err = gen_con( c, p )) != 0 )
    return err;

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_CONSTRUCT );
  return 0;
}

int gen_exe_decorator( Node* n, Program* p )
{
  Node* c = GetFirstChild( n );
  if( !c )
    return 0;

  DecoratorNodeData* nd = (DecoratorNodeData*)n->m_UserData;
  Decorator* d = n->m_Grist.m_Decorator.m_Decorator;

  Variable* t = FindVariableWithIdHash( d->m_Vars, hashlittle( "id" ) );
  int fid = t ? ValueAsInteger( *t ) : ~0;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_EXECUTE );

  int err;
  int jump_out = -1;

  t = FindVariableWithIdHash( d->m_Vars, hashlittle( "prune" ) );
  if( t && ValueAsBool( *t ) )
  {
    // Enter Debug scope
    p->m_I.PushDebugScope( p, n, ACT_PRUNE );

    //Setup the register for the data pointer
    err = setup_variable_registry( &nd->m_VD,
      n->m_Grist.m_Decorator.m_Arguments, p );
    if( err != 0 )
      return err;
    // Load bss register with bss pointer
    p->m_I.Push( INST_STORE_PB_IN_R, 1, nd->m_bssPos, 0 );
    // Load the register with the correct id
    p->m_I.Push( INST_LOAD_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid
        & 0x0000ffff );
    // Call the decorator prune function
    p->m_I.Push( INST_CALL_PRUN_FUN, 0, 1, 2 );

    // Exit Debug scope
    p->m_I.PopDebugScope( p, n, ACT_PRUNE );

    //Jump out if non-success.
    jump_out = p->m_I.Count();
    p->m_I.Push( INST_JABC_R_DIFF_C, 0xffffffff, E_NODE_SUCCESS, 0 );
  }

  //Generate child execution code
  if( (err = gen_exe( c, p )) != 0 )
    return err;

  t = FindVariableWithIdHash( d->m_Vars, hashlittle( "modify" ) );
  if( t && ValueAsBool( *t ) )
  {
    // Enter Debug scope
    p->m_I.PushDebugScope( p, n, ACT_MODIFY );

    //Copy return value to bss section
    p->m_I.Push( INST__STORE_R_IN_B, nd->m_bssModPos, 0, 0 );

    //Setup the register for the data pointer
    err = setup_variable_registry( &nd->m_VD,
      n->m_Grist.m_Decorator.m_Arguments, p );
    if( err != 0 )
      return err;
    // Load bss register with bss pointer
    p->m_I.Push( INST_STORE_PB_IN_R, 1, nd->m_bssPos, 0 );
    // Load the register with the correct id
    p->m_I.Push( INST_LOAD_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid
        & 0x0000ffff );
    //Call the decorator modify function
    p->m_I.Push( INST_CALL_MODI_FUN, 0, 1, 2 );

    // Exit Debug scope
    p->m_I.PopDebugScope( p, n, ACT_MODIFY );
  }

  if( jump_out != -1 )
    p->m_I.SetA1( jump_out, p->m_I.Count() );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_EXECUTE );

  return 0;
}

int gen_des_decorator( Node* n, Program* p )
{
  Node* c = GetFirstChild( n );
  if( !c )
    return 0;

  DecoratorNodeData* nd = (DecoratorNodeData*)n->m_UserData;
  Decorator* d = n->m_Grist.m_Decorator.m_Decorator;

  Variable* t = FindVariableWithIdHash( d->m_Vars, hashlittle( "id" ) );
  int fid = t ? ValueAsInteger( *t ) : ~0;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_DESTRUCT );

  int err;

  // Generate child destruction code
  if( (err = gen_des( c, p )) != 0 )
    return err;

  t = FindVariableWithIdHash( d->m_Vars, hashlittle( "destruct" ) );
  if( t && ValueAsBool( *t ) )
  {
    //Setup the register for the data pointer
    err = setup_variable_registry( &nd->m_VD,
      n->m_Grist.m_Decorator.m_Arguments, p );
    if( err != 0 )
      return err;
    // Load bss register with bss pointer
    p->m_I.Push( INST_STORE_PB_IN_R, 1, nd->m_bssPos, 0 );
    // Load the register with the correct id
    p->m_I.Push( INST_LOAD_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid
        & 0x0000ffff );
    // Call the decorator destruciton function
    p->m_I.Push( INST_CALL_DEST_FUN, 0, 1, 2 );
  }

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_DESTRUCT );

  return 0;
}

/*
 *
 * Action
 *
 */

struct ActionNodeData
{
  int m_bssPos;
  VariableGenerateData m_VD;
};

int gen_setup_action( Node* n, Program* p )
{
  //Alloc space needed for code generation
  ActionNodeData* nd = new ActionNodeData;

  //Store needed generation data in the node's UserData pointer
  n->m_UserData = nd;
  //Obtain action declaration
  Action* a = n->m_Grist.m_Action.m_Action;

  //Alloc bss-space for the callback function if it needs it.
  Variable* t = FindVariableWithIdHash( a->m_Vars, hashlittle( "bss" ) );
  int bss = t ? ValueAsInteger( *t ) : 0;
  if( bss > 0 )
    nd->m_bssPos = p->m_B.Push( bss, 4 );

  //Store the variable values in the data section.
  store_variables_in_data_section( &nd->m_VD, n->m_Grist.m_Action.m_Arguments,
    a->m_Args, p );

  return 0;
}

int gen_teardown_action( Node* n, Program* p )
{
  //Free the space used when generating code.
  delete ((ActionNodeData*)n->m_UserData);
  n->m_UserData = 0x0;

  return 0;
}

int gen_con_action( Node* n, Program* p )
{
  ActionNodeData* nd = (ActionNodeData*)n->m_UserData;
  //Obtain action declaration
  Action* a = n->m_Grist.m_Action.m_Action;
  //Get the actions function id
  Variable* t = FindVariableWithIdHash( a->m_Vars, hashlittle( "id" ) );
  int fid = t ? ValueAsInteger( *t ) : ~0;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_CONSTRUCT );

  int err;

  // Load bss section with pointers to the data section, for the variables
  err = generate_variable_instructions( &nd->m_VD,
    n->m_Grist.m_Action.m_Arguments, p );
  if( err != 0 )
    return err;

  t = FindVariableWithIdHash( a->m_Vars, hashlittle( "construct" ) );
  if( t && ValueAsBool( *t ) )
  {
    //Setup the register for the data pointer
    err = setup_variable_registry( &nd->m_VD, n->m_Grist.m_Action.m_Arguments,
      p );
    if( err != 0 )
      return err;
    // Load bss register with bss pointer
    p->m_I.Push( INST_STORE_PB_IN_R, 1, nd->m_bssPos, 0 );
    // Load the callback id register with the correct id
    p->m_I.Push( INST_LOAD_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid
        & 0x0000ffff );
    // Call the construction callback
    p->m_I.Push( INST_CALL_CONS_FUN, 0, 1, 2 );
  }

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_CONSTRUCT );

  return 0;
}

int gen_exe_action( Node* n, Program* p )
{
  ActionNodeData* nd = (ActionNodeData*)n->m_UserData;
  //Obtain action declaration
  Action* a = n->m_Grist.m_Action.m_Action;
  //Get the actions function id
  Variable* t = FindVariableWithIdHash( a->m_Vars, hashlittle( "id" ) );
  int fid = t ? ValueAsInteger( *t ) : ~0;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_EXECUTE );
  //Setup the register for the data pointer
  int err = setup_variable_registry( &nd->m_VD,
    n->m_Grist.m_Action.m_Arguments, p );
  if( err != 0 )
    return err;
  // Load bss register with bss pointer
  p->m_I.Push( INST_STORE_PB_IN_R, 1, nd->m_bssPos, 0 );
  // Load the callback id register with the correct id
  p->m_I.Push( INST_LOAD_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid
      & 0x0000ffff );
  // Call the destruction callback
  p->m_I.Push( INST_CALL_EXEC_FUN, 0, 1, 2 );
  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_EXECUTE );

  return 0;
}

int gen_des_action( Node* n, Program* p )
{
  ActionNodeData* nd = (ActionNodeData*)n->m_UserData;
  //Obtain action declaration
  Action* a = n->m_Grist.m_Action.m_Action;
  //Get the actions function id
  Variable* t = FindVariableWithIdHash( a->m_Vars, hashlittle( "id" ) );
  int fid = t ? ValueAsInteger( *t ) : ~0;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_DESTRUCT );

  t = FindVariableWithIdHash( a->m_Vars, hashlittle( "destruct" ) );
  if( t && ValueAsBool( *t ) )
  {
    //Setup the register for the data pointer
    int err = setup_variable_registry( &nd->m_VD,
      n->m_Grist.m_Action.m_Arguments, p );
    if( err != 0 )
      return err;
    // Load bss register with bss pointer
    p->m_I.Push( INST_STORE_PB_IN_R, 1, nd->m_bssPos, 0 );
    // Load the callback id register with the correct id
    p->m_I.Push( INST_LOAD_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid
        & 0x0000ffff );
    // Call the destruction callback
    p->m_I.Push( INST_CALL_DEST_FUN, 0, 1, 2 );
  }

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_DESTRUCT );

  return 0;
}

int store_variables_in_data_section( VariableGenerateData* vd, Variable* vars,
  Variable* dec, Program* p )
{
  if( !vars || !dec )
    return 0;

  vd->m_Data.clear();
  vd->m_bssStart = p->m_B.Push( sizeof(void*) * CountElementsInList( dec ),
    sizeof(void*) );

  DataSection& d = p->m_D;
  Variable* it;
  for( it = dec; it != 0x0; it = it->m_Next )
  {
    Variable* v = FindVariableWithIdHash( vars, it->m_Id.m_Hash );

    if( !v )
      return -1;

    switch( it->m_Type )
    {
    case E_VART_INTEGER:
      vd->m_Data.push_back( d.PushInteger( ValueAsInteger( *v ) ) );
      break;
    case E_VART_FLOAT:
      vd->m_Data.push_back( d.PushFloat( ValueAsFloat( *v ) ) );
      break;
    case E_VART_STRING:
      vd->m_Data.push_back( d.PushString( ValueAsString( *v )->m_Parsed ) );
      break;
    case E_VART_BOOL:
      vd->m_Data.push_back( d.PushInteger( ValueAsInteger( *v ) ) );
      break;
    default:
      return -1;
      break;
    }
  }

  return 0;
}
int generate_variable_instructions( VariableGenerateData* vd, Variable* vars,
  Program* p )
{
  IntVector::iterator it, it_e( vd->m_Data.end() );
  int i = 0;
  for( it = vd->m_Data.begin(); it != it_e; ++it, ++i )
  {
    //Store a pointer to a variable in the data section in the bss section.
    p->m_I.Push( INST_STORE_PD_IN_B, vd->m_bssStart + (sizeof(void*) * i), *it,
      0 );
  }
  return 0;
}

int setup_variable_registry( VariableGenerateData* vd, Variable* vars,
  Program* p )
{
  if( !vd->m_Data.empty() )
  {
    // Load the user data register with a pointer to the bss structure for variables
    p->m_I.Push( INST_STORE_PB_IN_R, 2, vd->m_bssStart, 0 );
  }
  else
  {
    //Load the user data register with a null pointer
    p->m_I.Push( INST_LOAD_REGISTRY, 2, 0, 0 );
  }
  return 0;
}

