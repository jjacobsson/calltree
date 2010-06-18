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

const int ACTION_CONSTRUCT_DBGLVL = 4;
const int ACTION_DESTRUCT_DBGLVL  = 4;
const int ACTION_EXECUTE_DBGLVL   = 1;

const int DECORATOR_CONSTRUCT_DBGLVL = 4;
const int DECORATOR_DESTRUCT_DBGLVL  = 4;
const int DECORATOR_EXECUTE_DBGLVL   = 1;

const int TREE_CONSTRUCT_DBGLVL = 5;
const int TREE_DESTRUCT_DBGLVL  = 5;
const int TREE_EXECUTE_DBGLVL   = 2;

const int STANDARD_NODE_CONSTRUCT_DBGLVL = 6;
const int STANDARD_NODE_DESTRUCT_DBGLVL  = 6;
const int STANDARD_NODE_EXECUTE_DBGLVL   = 3;

using namespace cb;

typedef std::vector<int> IntVector;

struct VariableGenerateData
{
  int m_bssStart;
  IntVector m_Data;
};

int memory_need_variables(
    Node* vars_n,
    Parameter* vars,
    NamedSymbol* dec_s,
    Parameter* dec
  );

int store_variables_in_data_section(
    VariableGenerateData* vd,
    Node* vars_n,
    Parameter* vars,
    NamedSymbol* dec_s,
    Parameter* dec,
    Program* p,
    int mo
  );

int generate_variable_instructions(
    VariableGenerateData* vd,
    Parameter* vars,
    Program* p
  );

int setup_variable_registry(
    VariableGenerateData* vd,
    Parameter* vars,
    Program* p
  );

int setup_gen( Node* n, Program* p, int mo )
{
  int r = -1;
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_SEQUENCE:
    r = gen_setup_sequence( n, p, mo );
    break;
  case E_GRIST_SELECTOR:
    r = gen_setup_selector( n, p, mo );
    break;
  case E_GRIST_PARALLEL:
    r = gen_setup_parallel( n, p, mo );
    break;
  case E_GRIST_DYN_SELECTOR:
    r = gen_setup_dynselector( n, p, mo );
    break;
  case E_GRIST_SUCCEED:
    r = gen_setup_succeed( n, p, mo );
    break;
  case E_GRIST_FAIL:
    r = gen_setup_fail( n, p, mo );
    break;
  case E_GRIST_WORK:
    r = gen_setup_work( n, p, mo );
    break;
  case E_GRIST_TREE:
    r = gen_setup_tree( n, p, mo );
    break;
  case E_GRIST_DECORATOR:
    r = gen_setup_decorator( n, p, mo );
    break;
  case E_GRIST_ACTION:
    r = gen_setup_action( n, p, mo );
    break;
  case E_GRIST_UNKOWN:
  case E_MAX_GRIST_TYPES:
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
  case E_GRIST_TREE:
    r = gen_teardown_tree( n, p );
    break;
  case E_GRIST_DECORATOR:
    r = gen_teardown_decorator( n, p );
    break;
  case E_GRIST_ACTION:
    r = gen_teardown_action( n, p );
    break;
  case E_GRIST_UNKOWN:
  case E_MAX_GRIST_TYPES:
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
  case E_GRIST_TREE:
    return gen_con_tree( n, p );
    break;
  case E_GRIST_DECORATOR:
    return gen_con_decorator( n, p );
    break;
  case E_GRIST_ACTION:
    return gen_con_action( n, p );
    break;
  case E_GRIST_UNKOWN:
  case E_MAX_GRIST_TYPES:
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
  case E_GRIST_TREE:
    return gen_exe_tree( n, p );
    break;
  case E_GRIST_DECORATOR:
    return gen_exe_decorator( n, p );
    break;
  case E_GRIST_ACTION:
    return gen_exe_action( n, p );
    break;
  case E_GRIST_UNKOWN:
  case E_MAX_GRIST_TYPES:
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
  case E_GRIST_TREE:
    return gen_des_tree( n, p );
    break;
  case E_GRIST_DECORATOR:
    return gen_des_decorator( n, p );
    break;
  case E_GRIST_ACTION:
    return gen_des_action( n, p );
    break;
  case E_GRIST_UNKOWN:
  case E_MAX_GRIST_TYPES:
    break;
  }
  return -1;
}

int calc_memory_need( Node* n )
{
  if( !n )
    return 0;

  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_SEQUENCE:
    return memory_need_sequence( n );
    break;
  case E_GRIST_SELECTOR:
    return memory_need_selector( n );
    break;
  case E_GRIST_PARALLEL:
    return memory_need_parallel( n );
    break;
  case E_GRIST_DYN_SELECTOR:
    return memory_need_dynselector( n );
    break;
  case E_GRIST_SUCCEED:
    return memory_need_succeed( n );
    break;
  case E_GRIST_FAIL:
    return memory_need_fail( n );
    break;
  case E_GRIST_WORK:
    return memory_need_work( n );
    break;
  case E_GRIST_TREE:
    return memory_need_tree( n );
    break;
  case E_GRIST_DECORATOR:
    return memory_need_decorator( n );
    break;
  case E_GRIST_ACTION:
    return memory_need_action( n );
    break;
  case E_GRIST_UNKOWN:
  case E_MAX_GRIST_TYPES:
    break;
  }
  return -1;
}

/*
 *
 * BehaviorTree
 *
 */

struct BehaviorTreeNodeData
{
  int m_bss_Destruct;
  int m_bss_Return;
};

int gen_setup_btree( BehaviorTree* t, Program* p, int mo )
{
  BehaviorTreeNodeData* nd = new BehaviorTreeNodeData;

  //Alloc storage area for "destroy" command.
  nd->m_bss_Destruct = mo;
  mo += sizeof( int );
  //Alloc storage area for child-node return value.
  nd->m_bss_Return = mo;
  mo += sizeof( int );

  t->m_UserData = nd;

  return setup_gen( t->m_Root, p, mo );
}

int gen_teardown_btree( BehaviorTree* t, Program* p )
{
  BehaviorTreeNodeData* nd = (BehaviorTreeNodeData*)t->m_UserData;
  delete nd;
  t->m_UserData = 0x0;
  return teardown_gen( t->m_Root, p );
}

int gen_btree( BehaviorTree* t, Program* p )
{
  return -1;
  /*
  if( !t || !p )
    return -1;

  int err;

  BehaviorTreeNodeData* nd = (BehaviorTreeNodeData*)t->m_UserData;

  int patch_jmp_to_exec;
  int patch_jmp_to_dest;
  int patch_jmp_to_exit[2];

  //Store the jump instruction to patch
  patch_jmp_to_dest = p->m_I.Count();

  //Jump to destruction code if set to do so.
  p->m_I.Push( INST_JABC_C_EQUA_B, 0xffffffff, ACT_DESTRUCT, nd->m_bss_Destruct );

  //Store the jump instruction to patch
  patch_jmp_to_exec = p->m_I.Count();

  //Jump past construction code if tree is already running
  p->m_I.Push( INST_JABC_C_EQUA_B, 0xffffffff, E_NODE_WORKING, nd->m_bss_Return );

  //Generate tree construction code
  if( (err = gen_con( t->m_Root, p )) != 0 )
    return err;

  //Patch jump past construction code instruction
  p->m_I.SetA1( patch_jmp_to_exec, p->m_I.Count() );

  //Generate tree execution code
  if( (err = gen_exe( t->m_Root, p )) != 0 )
    return err;

  //Store return value in bss.
  p->m_I.Push( INST__STORE_R_IN_B, nd->m_bss_Return, 0, 0 );

  //Jump past destruction code if tree is working
  patch_jmp_to_exit[0] = p->m_I.Count();
  p->m_I.Push( INST_JABC_R_EQUA_C, 0xffffffff, E_NODE_WORKING, 0 );

  //This is a bit tricky. If we get here the tree is *not* working,
  //but we need the destruction code to think that it is.
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_Return, E_NODE_WORKING, 0 );

  //Patch jump to destruction code
  p->m_I.SetA1( patch_jmp_to_dest, p->m_I.Count() );

  //Reset the destruction command
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_Destruct, 0, 0 );

  //Store the jump to exit patch
  patch_jmp_to_exit[1] = p->m_I.Count();

  //Skip destruction if the tree state is anything other than E_NODE_WORKING
  p->m_I.Push( INST_JABC_C_DIFF_B, 0xffffffff, E_NODE_WORKING, nd->m_bss_Return );

  //Generate destruction code
  if( (err = gen_des( t->m_Root, p )) != 0 )
    return err;

  //Set the tree-state to uninitialized.
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_Return, E_NODE_UNDEFINED, 0 );

  //Patch jump to exit code instructions
  p->m_I.SetA1( patch_jmp_to_exit[0], p->m_I.Count() );
  p->m_I.SetA1( patch_jmp_to_exit[1], p->m_I.Count() );

  //Return out
  p->m_I.Push( INST_SCRIPT_R, 0, 0, 0 );

  return 0;
  */
}

int memory_need_btree( BehaviorTree* bt )
{
  int bnc = calc_memory_need( bt->m_Root );
  if( bnc < 0 )
    return bnc;
  return bnc + sizeof(int) * 2;
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

int gen_setup_sequence( Node* n, Program* p, int mo )
{
  //Alloc space needed for code generation
  SequenceNodeData* nd = new SequenceNodeData;
  //Store needed generation data in the node's UserData pointer
  n->m_UserData = nd;

  //Alloc storage area for function call stack.
  nd->m_bss_JumpBackTarget = mo;
  mo += sizeof( int );
  //Alloc storage area for re-entry instruction
  nd->m_bss_ReEntry = mo;
  mo += sizeof( int );

  int maximum = mo;
  Node* c = get_first_child( n );
  while( c )
  {
    int cm = setup_gen( c, p, mo );
    if( cm < 0 )
      return cm;

    if( cm > maximum )
      maximum = cm;

    c = c->m_Next;
  }

  return maximum;
}

int gen_teardown_sequence( Node* n, Program* p )
{
  //Free the space used when generating code.
  delete ((SequenceNodeData*)n->m_UserData);
  n->m_UserData = 0x0;

  int r = 0;
  Node* c = get_first_child( n );
  while( c && r == 0 )
  {
    r = teardown_gen( c, p );
    c = c->m_Next;
  }
  return r;
}

int gen_con_sequence( Node* n, Program* p )
{
  /*
  SequenceNodeData* nd = (SequenceNodeData*)n->m_UserData;
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_CONSTRUCT, STANDARD_NODE_CONSTRUCT_DBGLVL );

  //Set jump-back pointer value to uninitialized
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_JumpBackTarget, 0xffffffff, 0 );

  //Set re-entry pointer value to uninitialized
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_ReEntry, 0xffffffff, 0 );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_CONSTRUCT, STANDARD_NODE_CONSTRUCT_DBGLVL );
  */
  return 0;
}

int gen_exe_sequence( Node* n, Program* p )
{
  /*
  SequenceNodeData* nd = (SequenceNodeData*)n->m_UserData;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_EXECUTE, STANDARD_NODE_EXECUTE_DBGLVL );

  IntVector exit_working;
  IntVector exit_fail;
  IntVector dest_jumps;

  //Jump to re-entry point if set
  p->m_I.Push( INST_JABB_C_DIFF_B, nd->m_bss_ReEntry, 0xffffffff,
    nd->m_bss_ReEntry );
  int err;
  Node* c = get_first_child( n );
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
  c = get_first_child( n );
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
  p->m_I.PopDebugScope( p, n, ACT_EXECUTE, STANDARD_NODE_EXECUTE_DBGLVL );
  */
  return 0;
}

int gen_des_sequence( Node* n, Program* p )
{
  /*
  SequenceNodeData* nd = (SequenceNodeData*)n->m_UserData;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_DESTRUCT, STANDARD_NODE_DESTRUCT_DBGLVL );

  //Jump past destruction code if m_bss_JumpBackTarget is uninitialized
  p->m_I.Push( INST_JABC_C_EQUA_B, p->m_I.Count() + 2, 0xffffffff,
    nd->m_bss_JumpBackTarget );
  //Jump to destruction code and set the jump-back at the same time
  p->m_I.Push( INST_JABB_S_C_IN_B, nd->m_bss_JumpBackTarget,
    nd->m_bss_JumpBackTarget, p->m_I.Count() + 1 );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_DESTRUCT, STANDARD_NODE_DESTRUCT_DBGLVL );
  */
  return 0;
}

int memory_need_sequence( Node* n )
{
  int max_child = 0;
  int self = sizeof(int) * 2;
  Node* c = get_first_child( n );
  while( c )
  {
    int curr_child = calc_memory_need( c );
    if( curr_child < 0 )
      return curr_child;

    if( max_child < curr_child )
      max_child = curr_child;

    c = c->m_Next;
  }
  return self + max_child;
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

int gen_setup_selector( Node* n, Program* p, int mo )
{
  //Alloc space needed for code generation
  SelectorNodeData* nd = new SelectorNodeData;

  //Store needed generation data in the node's UserData pointer
  n->m_UserData = nd;

  //Alloc storage area for function call stack.
  nd->m_bss_JumpBackTarget = mo;
  mo += sizeof( int );

  //Alloc storage area for re-entry instruction
  nd->m_bss_ReEntry = mo;
  mo += sizeof( int );

  int maximum = mo;
  Node* c = get_first_child( n );
  while( c )
  {
    int cm = setup_gen( c, p, mo );
    if( cm < 0 )
      return cm;

    if( cm > maximum )
      maximum = cm;

    c = c->m_Next;
  }

  return maximum;
}

int gen_teardown_selector( Node* n, Program* p )
{
  //Free the space used when generating code.
  delete ((SelectorNodeData*)n->m_UserData);
  n->m_UserData = 0x0;

  int r = 0;
  Node* c = get_first_child( n );
  while( c && r == 0 )
  {
    r = teardown_gen( c, p );
    c = c->m_Next;
  }
  return r;
}

int gen_con_selector( Node* n, Program* p )
{
  /*
  SelectorNodeData* nd = (SelectorNodeData*)n->m_UserData;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_CONSTRUCT, STANDARD_NODE_CONSTRUCT_DBGLVL );

  //Set re-entry pointer value to uninitialized
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_ReEntry, 0xffffffff, 0 );

  //Set re-entry pointer value to uninitialized
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_JumpBackTarget, 0xffffffff, 0 );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_CONSTRUCT, STANDARD_NODE_CONSTRUCT_DBGLVL );
  */
  return 0;
}

int gen_exe_selector( Node* n, Program* p )
{
  /*
  SelectorNodeData* nd = (SelectorNodeData*)n->m_UserData;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_EXECUTE, STANDARD_NODE_EXECUTE_DBGLVL );

  IntVector exit_working;
  IntVector exit_success;
  IntVector dest_jumps;

  //Jump to re-entry point if set
  p->m_I.Push( INST_JABB_C_DIFF_B, nd->m_bss_ReEntry, 0xffffffff,
    nd->m_bss_ReEntry );

  int err;
  Node* c = get_first_child( n );
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
  c = get_first_child( n );
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
  p->m_I.PopDebugScope( p, n, ACT_EXECUTE, STANDARD_NODE_EXECUTE_DBGLVL );
  */
  return 0;
}

int gen_des_selector( Node* n, Program* p )
{
  /*
  SelectorNodeData* nd = (SelectorNodeData*)n->m_UserData;
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_DESTRUCT, STANDARD_NODE_DESTRUCT_DBGLVL );

  //Jump past destruction code if nd->m_bss_JumpBackTarget is uninitialized
  p->m_I.Push( INST_JABC_C_EQUA_B, p->m_I.Count() + 2, 0xffffffff,
    nd->m_bss_JumpBackTarget );
  //Jump to destruction code and set the jump-back at the same time
  p->m_I.Push( INST_JABB_S_C_IN_B, nd->m_bss_JumpBackTarget,
    nd->m_bss_JumpBackTarget, p->m_I.Count() + 1 );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_DESTRUCT, STANDARD_NODE_DESTRUCT_DBGLVL );
  */
  return 0;
}

int memory_need_selector( Node* n )
{
  int max_child = 0;
  int self = sizeof(int) * 2;
  Node* c = get_first_child( n );
  while( c )
  {
    int curr_child = calc_memory_need( c );
    if( curr_child < 0 )
      return curr_child;

    if( max_child < curr_child )
      max_child = curr_child;

    c = c->m_Next;
  }
  return self + max_child;
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

int gen_setup_parallel( Node* n, Program* p, int mo )
{
  //Alloc space needed for code generation
  ParallelNodeData* nd = new ParallelNodeData;

  //Store needed generation data in the node's UserData pointer
  n->m_UserData = nd;

  //Alloc storage space for the success counter
  nd->m_bss_SuccessCounter = mo;
  mo += sizeof( int );

  int r = 0;
  Node* c = get_first_child( n );
  while( c )
  {
    r = setup_gen( c, p, mo );
    if( r < 0 )
      return r;
    mo = r;
    c = c->m_Next;
  }
  return mo;
}

int gen_teardown_parallel( Node* n, Program* p )
{
  //Free the space used when generating code.
  delete ((ParallelNodeData*)n->m_UserData);
  n->m_UserData = 0x0;

  int r = 0;
  Node* c = get_first_child( n );
  while( c && r == 0 )
  {
    r = teardown_gen( c, p );
    c = c->m_Next;
  }
  return r;
}

int gen_con_parallel( Node* n, Program* p )
{
  //ParallelNodeData* nd = (ParallelNodeData*)n->m_UserData;
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_CONSTRUCT, STANDARD_NODE_CONSTRUCT_DBGLVL );

  int err;
  Node* c = get_first_child( n );
  while( c )
  {
    //call child-node construction code
    if( (err = gen_con( c, p )) != 0 )
      return err;
    //Iterate
    c = c->m_Next;
  }

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_CONSTRUCT, STANDARD_NODE_CONSTRUCT_DBGLVL );
  return 0;
}

int gen_exe_parallel( Node* n, Program* p )
{
  /*
  ParallelNodeData* nd = (ParallelNodeData*)n->m_UserData;
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_EXECUTE, STANDARD_NODE_EXECUTE_DBGLVL );

  //Set the success counter to 0
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_SuccessCounter, 0, 0 );

  IntVector exit_fail;

  int i = 0, err;
  Node* c = get_first_child( n );
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
  p->m_I.PopDebugScope( p, n, ACT_EXECUTE, STANDARD_NODE_EXECUTE_DBGLVL );
  */
  return 0;
}

int gen_des_parallel( Node* n, Program* p )
{
  //ParallelNodeData* nd = (ParallelNodeData*)n->m_UserData;
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_DESTRUCT, STANDARD_NODE_DESTRUCT_DBGLVL );

  int err;
  Node* c = get_first_child( n );
  while( c )
  {
    //call child-node destruction code
    if( (err = gen_des( c, p )) != 0 )
      return err;
    //Iterate
    c = c->m_Next;
  }

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_DESTRUCT, STANDARD_NODE_DESTRUCT_DBGLVL );
  return 0;
}

int memory_need_parallel( Node* n )
{
  int r = sizeof(int) * 1;
  Node* c = get_first_child( n );
  while( c )
  {
    int t = calc_memory_need( c );
    if( t < 0 )
      return t;
    r += t;
    c = c->m_Next;
  }
  return r;
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

int gen_setup_dynselector( Node* n, Program* p, int mo )
{
  //Alloc space needed for code generation
  DynamicSelectorNodeData* nd = new DynamicSelectorNodeData;

  //Store needed generation data in the node's UserData pointer
  n->m_UserData = nd;

  //Alloc storage area in bss
  nd->m_bss_NewBranch = mo; mo += sizeof( int );
  nd->m_bss_OldBranch = mo; mo += sizeof( int );
  nd->m_bss_JumpBackTarget = mo; mo += sizeof( int );
  nd->m_bss_RunningChild = mo; mo += sizeof(int) * count_children( n );

  int r = 0;
  Node* c = get_first_child( n );
  while( c )
  {
    r = setup_gen( c, p, mo );
    if( r < 0 )
      return r;
    mo = r;
    c = c->m_Next;
  }
  return mo;
}

int gen_teardown_dynselector( Node* n, Program* p )
{
  //Free the space used when generating code.
  delete ((DynamicSelectorNodeData*)n->m_UserData);
  n->m_UserData = 0x0;

  int r = 0;
  Node* c = get_first_child( n );
  while( c && r == 0 )
  {
    r = teardown_gen( c, p );
    c = c->m_Next;
  }
  return r;
}

int gen_con_dynselector( Node* n, Program* p )
{
  /*
  DynamicSelectorNodeData* nd = (DynamicSelectorNodeData*)n->m_UserData;
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_CONSTRUCT, STANDARD_NODE_CONSTRUCT_DBGLVL );
  int i = 0;
  Node* c = get_first_child( n );
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
  p->m_I.PopDebugScope( p, n, ACT_CONSTRUCT, STANDARD_NODE_CONSTRUCT_DBGLVL );
  */
  return 0;
}

int gen_exe_dynselector( Node* n, Program* p )
{
  /*
  DynamicSelectorNodeData* nd = (DynamicSelectorNodeData*)n->m_UserData;
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_EXECUTE, STANDARD_NODE_EXECUTE_DBGLVL );

  IntVector exit_jumps, true_exit_jumps, cons_jumps, exec_jumps, dest_jumps;
  int i = 0;
  Node* c = get_first_child( n );
  while( c )
  {
    //calculate where in the bss this child's return value should be stored
    int bss_current_child = nd->m_bss_RunningChild + (i * sizeof(int));

    //Set jump back after construction target.
    p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_JumpBackTarget, p->m_I.Count()
        + 2, 0 );
    //Jump to construction code *if* this child is E_NODE_UNDEFINED.
    cons_jumps.push_back( p->m_I.Count() );
    p->m_I.Push( INST_JABC_C_EQUA_B, 0xffffffff, E_NODE_UNDEFINED,
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
  c = get_first_child( n );
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
    //Jump past the destruct code if the branch state is undefined
    int patch = p->m_I.Count();
    p->m_I.Push( INST_JABC_C_EQUA_B, 0xffffffff, E_NODE_UNDEFINED, bss_current_child );
    //call child-node destruction code
    if( (err = gen_des( c, p )) != 0 )
      return err;
    //Set childs stored return value to undefined
    p->m_I.Push( INST__STORE_C_IN_B, bss_current_child, E_NODE_UNDEFINED, 0 );
    //Patch the jump
    p->m_I.SetA1( patch, p->m_I.Count() );
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

  //Jump past "destroy old branch" if "old branch" is uninitialized
  int patch = p->m_I.Count();
  p->m_I.Push( INST_JABC_C_EQUA_B, 0xffffffff, 0xffffffff, nd->m_bss_OldBranch );
  //set jump back after destruction target
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_JumpBackTarget,
    p->m_I.Count() + 2, 0 );
  //Jump to destruction code of "old branch" *if* it differs from "new branch"
  p->m_I.Push( INST_JABB_B_DIFF_B, nd->m_bss_OldBranch, nd->m_bss_NewBranch,
    nd->m_bss_OldBranch );
  //Patch the jump past
  p->m_I.SetA1( patch, p->m_I.Count() );
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
  p->m_I.PopDebugScope( p, n, ACT_EXECUTE, STANDARD_NODE_EXECUTE_DBGLVL );
  */
  return 0;
}

int gen_des_dynselector( Node* n, Program* p )
{
  /*
  DynamicSelectorNodeData* nd = (DynamicSelectorNodeData*)n->m_UserData;
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_DESTRUCT, STANDARD_NODE_DESTRUCT_DBGLVL );

  //set jump back after destruction target
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_JumpBackTarget,
    p->m_I.Count() + 2, 0 );
  //Jump to destruction code of "old branch" if it is set to a valid value
  p->m_I.Push( INST_JABB_C_DIFF_B, nd->m_bss_OldBranch, 0xffffffff,
    nd->m_bss_OldBranch );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_DESTRUCT, STANDARD_NODE_DESTRUCT_DBGLVL );
  */
  return 0;
}

int memory_need_dynselector( Node* n )
{
  int r = sizeof(int) * 3;
  r += sizeof( int ) * count_children( n );
  Node* c = get_first_child( n );
  while( c )
  {
    int t = calc_memory_need( c );
    if( t < 0 )
      return t;
    r += t;
    c = c->m_Next;
  }
  return r;
}

/*
 *
 * Succeed
 *
 */

int gen_setup_succeed( Node* , Program*, int mo )
{
  return mo;
}

int gen_teardown_succeed( Node*, Program* )
{
  return 0;
}

int gen_con_succeed( Node*, Program* )
{
  return 0;
}

int gen_exe_succeed( Node*, Program* p )
{
  //p->m_I.Push( INST__STORE_C_IN_R, E_NODE_SUCCESS, 0, 0 );
  return 0;
}

int gen_des_succeed( Node*, Program* )
{
  return 0;
}

int memory_need_succeed( Node* n )
{
  return 0;
}


/*
 *
 * Fail
 *
 */

int gen_setup_fail( Node*, Program*, int mo )
{
  return mo;
}
int gen_teardown_fail( Node*, Program* )
{
  return 0;
}
int gen_con_fail( Node*, Program* )
{
  return 0;
}
int gen_exe_fail( Node*, Program* p )
{
  //p->m_I.Push( INST__STORE_C_IN_R, E_NODE_FAIL, 0, 0 );
  return 0;
}
int gen_des_fail( Node*, Program* )
{
  return 0;
}

int memory_need_fail( Node* n )
{
  return 0;
}

/*
 *
 * Work
 *
 */

int gen_setup_work( Node*, Program*, int mo )
{
  return mo;
}

int gen_teardown_work( Node*, Program* )
{
  return 0;
}

int gen_con_work( Node*, Program* )
{
  return 0;
}

int gen_exe_work( Node*, Program* p )
{
  //p->m_I.Push( INST__STORE_C_IN_R, E_NODE_WORKING, 0, 0 );
  return 0;
}

int gen_des_work( Node*, Program* )
{
  return 0;
}

int memory_need_work( Node* n )
{
  return 0;
}

/*
 *
 * Sub Tree's
 *
 */

struct TreeNodeData
{
  int m_bss_Call;
  int m_CallPatch;
  int m_DestructPatch;
};

int gen_setup_tree( Node* n, Program* p, int mo )
{
  //Alloc space needed for code generation
  TreeNodeData* nd = new TreeNodeData;

  //Store needed generation data in the node's UserData pointer
  n->m_UserData = nd;

  //"Alloc" space needed for the "call"
  nd->m_bss_Call = mo; mo += sizeof( CallFrame );

  //"Alloc" space needed for the called tree
  mo += memory_need_btree( n->m_Grist.m_Tree.m_Tree );

  //Append this tree to the list of tree's that need generation.
  BehaviorTreeList* btl = p->m_First;
  BehaviorTree* t = n->m_Grist.m_Tree.m_Tree;
  while( btl && btl->m_Next )
  {
    if( btl->m_Tree == t )
      return mo;
    btl = btl->m_Next;
  }

  if( btl->m_Tree == t )
    return mo;

  btl->m_Next = new BehaviorTreeList;
  btl = btl->m_Next;
  btl->m_Next = 0x0;
  btl->m_Tree = t;

  return mo;
}

int gen_teardown_tree( Node* n, Program* p )
{
  //Free the space used when generating code.
  delete ((TreeNodeData*)n->m_UserData);
  n->m_UserData = 0x0;
  return 0;
}

int gen_con_tree( Node* n, Program* p )
{
  return -1;
  /*
  TreeNodeData* nd = ((TreeNodeData*)n->m_UserData);
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_CONSTRUCT, TREE_CONSTRUCT_DBGLVL );
  //Clear the tree argument
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_Call + sizeof(CallFrame), 0, 0 );
  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_CONSTRUCT, TREE_CONSTRUCT_DBGLVL );
  return 0;
  */
}

int gen_exe_tree( Node* n, Program* p )
{
  return -1;
  /*
  TreeNodeData* nd = ((TreeNodeData*)n->m_UserData);
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_EXECUTE, TREE_EXECUTE_DBGLVL );
  //Store the call instruction to patch.
  nd->m_CallPatch = p->m_I.Count();
  //Make the call
  p->m_I.Push( INST_SCRIPT_C, 0xffffffff, nd->m_bss_Call, 0 );
  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_EXECUTE, TREE_EXECUTE_DBGLVL );
  return 0;
  */
}

int gen_des_tree( Node* n, Program* p )
{
  return -1;
  /*
  TreeNodeData* nd = ((TreeNodeData*)n->m_UserData);
  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_DESTRUCT, TREE_DESTRUCT_DBGLVL );
  //Set the tree argument to destroy
  p->m_I.Push( INST__STORE_C_IN_B, nd->m_bss_Call + sizeof(CallFrame), ACT_DESTRUCT, 0 );
  //Store the call instruction to patch.
  nd->m_DestructPatch = p->m_I.Count();
  //Make the call
  p->m_I.Push( INST_SCRIPT_C, 0xffffffff, nd->m_bss_Call, 0 );
  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_DESTRUCT, TREE_DESTRUCT_DBGLVL );
  return 0;
  */
}

int memory_need_tree( Node* n )
{
  int bnt = memory_need_btree( n->m_Grist.m_Tree.m_Tree );
  if( bnt < 0 )
    return bnt;
  return sizeof( CallFrame ) + bnt;
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
  bool m_usesBss;
  VariableGenerateData m_VD;
};

int gen_setup_decorator( Node* n, Program* p, int mo )
{
  Node* c = get_first_child( n );
  if( !c )
    return 0;

  //Alloc space needed for code generation
  DecoratorNodeData* nd = new DecoratorNodeData;

  //init the bss members
  nd->m_bssPos      = 0;
  nd->m_bssModPos   = 0;
  nd->m_usesBss     = false;

  //Store needed generation data in the node's UserData pointer
  n->m_UserData = nd;

  //Get the decorator declaration.
  Decorator* d = n->m_Grist.m_Decorator.m_Decorator;

  Parameter* t = find_by_hash( d->m_Options, hashlittle( "bss" ) );
  int bss_need = t ? as_integer( *t ) : 0;
  t = find_by_hash( d->m_Options, hashlittle( "modify" ) );
  if( t && as_bool( *t ) )
    bss_need += sizeof( int );

  if( bss_need > 0 )
  {
    nd->m_bssPos = mo; mo += bss_need;
    nd->m_bssModPos = (nd->m_bssPos + bss_need) - sizeof( int );
    nd->m_usesBss   = true;
  }

  {
    NamedSymbol tns;
    tns.m_Type = E_ST_DECORATOR;
    tns.m_Symbol.m_Decorator = d;
    //Store the variable values in the data section.
    mo = store_variables_in_data_section( &nd->m_VD, n,
      n->m_Grist.m_Decorator.m_Parameters, &tns, d->m_Declarations, p, mo );

    if( mo < 0 )
      return mo;
  }

  return setup_gen( c, p, mo );
}

int gen_teardown_decorator( Node* n, Program* p )
{
  Node* c = get_first_child( n );
  if( !c )
    return 0;

  //Free the space used when generating code.
  delete ((DecoratorNodeData*)n->m_UserData);
  n->m_UserData = 0x0;

  return teardown_gen( c, p );
}

int gen_con_decorator( Node* n, Program* p )
{
  /*
  Node* c = get_first_child( n );
  if( !c )
    return 0;

  DecoratorNodeData* nd = (DecoratorNodeData*)n->m_UserData;

  Decorator* d = n->m_Grist.m_Decorator.m_Decorator;

  Parameter* t = find_by_hash( d->m_Options, hashlittle( "id" ) );
  int fid = t ? as_integer( *t ) : d->m_Id.m_Hash;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_CONSTRUCT, DECORATOR_CONSTRUCT_DBGLVL );

  int err;

  // Load bss section with pointers to the data section, for the variables
  err = generate_variable_instructions( &nd->m_VD,
    n->m_Grist.m_Decorator.m_Parameters, p );
  if( err != 0 )
    return err;

  t = find_by_hash( d->m_Options, hashlittle( "construct" ) );
  if( t && as_bool( *t ) )
  {
    //Setup the register for the data pointer
    err = setup_variable_registry( &nd->m_VD,
      n->m_Grist.m_Decorator.m_Parameters, p );
    if( err != 0 )
      return err;
    if( nd->m_usesBss )
    {
      // Load bss register with bss pointer
      p->m_I.Push( INST_STORE_PB_IN_R, 1, nd->m_bssPos, 0 );
    }
    else
    {
      // Reset the bss register
      p->m_I.Push( INST__SET_REGISTRY, 1, 0, 0 );
    }
    // Load the register with the correct id
    p->m_I.Push( INST__SET_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid
        & 0x0000ffff );
    // Call the decorator construction function
    p->m_I.Push( INST_CALL_CONS_FUN, 0, 1, 2 );
  }

  //Generate child construction code
  if( (err = gen_con( c, p )) != 0 )
    return err;

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_CONSTRUCT, DECORATOR_CONSTRUCT_DBGLVL );
  */
  return 0;
}

int gen_exe_decorator( Node* n, Program* p )
{
  /*
  Node* c = get_first_child( n );
  if( !c )
    return 0;

  DecoratorNodeData* nd = (DecoratorNodeData*)n->m_UserData;
  Decorator* d = n->m_Grist.m_Decorator.m_Decorator;

  Parameter* t = find_by_hash( d->m_Options, hashlittle( "id" ) );
  int fid = t ? as_integer( *t ) : d->m_Id.m_Hash;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_EXECUTE, STANDARD_NODE_EXECUTE_DBGLVL );

  int err;
  int jump_out = -1;

  t = find_by_hash( d->m_Options, hashlittle( "prune" ) );
  if( t && as_bool( *t ) )
  {
    // Enter Debug scope
    p->m_I.PushDebugScope( p, n, ACT_PRUNE, DECORATOR_EXECUTE_DBGLVL );

    //Setup the register for the data pointer
    err = setup_variable_registry( &nd->m_VD,
      n->m_Grist.m_Decorator.m_Parameters, p );
    if( err != 0 )
      return err;
    if( nd->m_usesBss )
    {
      // Load bss register with bss pointer
      p->m_I.Push( INST_STORE_PB_IN_R, 1, nd->m_bssPos, 0 );
    }
    else
    {
      // Reset the bss register
      p->m_I.Push( INST__SET_REGISTRY, 1, 0, 0 );
    }
    // Load the register with the correct id
    p->m_I.Push( INST__SET_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid
        & 0x0000ffff );
    // Call the decorator prune function
    p->m_I.Push( INST_CALL_PRUN_FUN, 0, 1, 2 );

    // Exit Debug scope
    p->m_I.PopDebugScope( p, n, ACT_PRUNE, DECORATOR_EXECUTE_DBGLVL );

    //Jump out if non-success.
    jump_out = p->m_I.Count();
    p->m_I.Push( INST_JABC_R_DIFF_C, 0xffffffff, E_NODE_SUCCESS, 0 );
  }

  //Generate child execution code
  if( (err = gen_exe( c, p )) != 0 )
    return err;

  t = find_by_hash( d->m_Options, hashlittle( "modify" ) );
  if( t && as_bool( *t ) )
  {
    // Enter Debug scope
    p->m_I.PushDebugScope( p, n, ACT_MODIFY, DECORATOR_EXECUTE_DBGLVL );

    //Copy return value to bss section
    p->m_I.Push( INST__STORE_R_IN_B, nd->m_bssModPos, 0, 0 );

    //Setup the register for the data pointer
    err = setup_variable_registry( &nd->m_VD,
      n->m_Grist.m_Decorator.m_Parameters, p );
    if( err != 0 )
      return err;
    if( nd->m_usesBss )
    {
      // Load bss register with bss pointer
      p->m_I.Push( INST_STORE_PB_IN_R, 1, nd->m_bssPos, 0 );
    }
    else
    {
      // Reset the bss register
      p->m_I.Push( INST__SET_REGISTRY, 1, 0, 0 );
    }
    // Load the register with the correct id
    p->m_I.Push( INST__SET_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid
        & 0x0000ffff );
    //Call the decorator modify function
    p->m_I.Push( INST_CALL_MODI_FUN, 0, 1, 2 );

    // Exit Debug scope
    p->m_I.PopDebugScope( p, n, ACT_MODIFY, DECORATOR_EXECUTE_DBGLVL );
  }

  if( jump_out != -1 )
    p->m_I.SetA1( jump_out, p->m_I.Count() );

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_EXECUTE, DECORATOR_EXECUTE_DBGLVL );
  */
  return 0;
}

int gen_des_decorator( Node* n, Program* p )
{
  /*
  Node* c = get_first_child( n );
  if( !c )
    return 0;

  DecoratorNodeData* nd = (DecoratorNodeData*)n->m_UserData;
  Decorator* d = n->m_Grist.m_Decorator.m_Decorator;

  Parameter* t = find_by_hash( d->m_Options, hashlittle( "id" ) );
  int fid = t ? as_integer( *t ) : d->m_Id.m_Hash;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_DESTRUCT, DECORATOR_DESTRUCT_DBGLVL );

  int err;

  // Generate child destruction code
  if( (err = gen_des( c, p )) != 0 )
    return err;

  t = find_by_hash( d->m_Options, hashlittle( "destruct" ) );
  if( t && as_bool( *t ) )
  {
    //Setup the register for the data pointer
    err = setup_variable_registry( &nd->m_VD,
      n->m_Grist.m_Decorator.m_Parameters, p );
    if( err != 0 )
      return err;
    if( nd->m_usesBss )
    {
      // Load bss register with bss pointer
      p->m_I.Push( INST_STORE_PB_IN_R, 1, nd->m_bssPos, 0 );
    }
    else
    {
      // Reset the bss register
      p->m_I.Push( INST__SET_REGISTRY, 1, 0, 0 );
    }
    // Load the register with the correct id
    p->m_I.Push( INST__SET_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid
        & 0x0000ffff );
    // Call the decorator destruciton function
    p->m_I.Push( INST_CALL_DEST_FUN, 0, 1, 2 );
  }

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_DESTRUCT, DECORATOR_DESTRUCT_DBGLVL );
  */
  return 0;
}

int memory_need_decorator( Node* n )
{
  //Get the decorator declaration.
  Decorator* d = n->m_Grist.m_Decorator.m_Decorator;

  Parameter* t = find_by_hash( d->m_Options, hashlittle( "bss" ) );
  int bss = t ? as_integer( *t ) : 0;
  t = find_by_hash( d->m_Options, hashlittle( "modify" ) );
  if( t && as_bool( *t ) )
    bss += 4;

  NamedSymbol tns;
  tns.m_Type = E_ST_DECORATOR;
  tns.m_Symbol.m_Decorator = n->m_Grist.m_Decorator.m_Decorator;

  int bnv = memory_need_variables(
    n,
    n->m_Grist.m_Decorator.m_Parameters,
    &tns,
    n->m_Grist.m_Decorator.m_Decorator->m_Declarations
  );

  if( bnv < 0 )
    return bnv;

  int bnc = calc_memory_need( n->m_Grist.m_Decorator.m_Child );
  if( bnc < 0 )
    return bnc;

  return bss + bnv + bnc;
}

/*
 *
 * Action
 *
 */

struct ActionNodeData
{
  int m_bssPos;
  bool m_usesBss;
  VariableGenerateData m_VD;
};

int gen_setup_action( Node* n, Program* p, int mo )
{
  //Alloc space needed for code generation
  ActionNodeData* nd = new ActionNodeData;
  //Set the bss pointer to zero.
  nd->m_bssPos = 0;
  nd->m_usesBss = false;
  //Store needed generation data in the node's UserData pointer
  n->m_UserData = nd;
  //Obtain action declaration
  Action* a = n->m_Grist.m_Action.m_Action;

  //Alloc bss-space for the callback function if it needs it.
  Parameter* t = find_by_hash( a->m_Options, hashlittle( "bss" ) );
  int bss = t ? as_integer( *t ) : 0;
  if( bss > 0 )
  {
    nd->m_bssPos  = mo; mo += bss;
    nd->m_usesBss   = true;
  }

  {
    NamedSymbol tns;
    tns.m_Type = E_ST_ACTION;
    tns.m_Symbol.m_Action = a;
    //Store the variable values in the data section.
    mo = store_variables_in_data_section( &nd->m_VD, n,
      n->m_Grist.m_Action.m_Parameters, &tns, a->m_Declarations, p, mo );
  }

  return mo;
}

int gen_teardown_action( Node* n, Program* )
{
  //Free the space used when generating code.
  delete ((ActionNodeData*)n->m_UserData);
  n->m_UserData = 0x0;

  return 0;
}

int gen_con_action( Node* n, Program* p )
{
  /*
  ActionNodeData* nd = (ActionNodeData*)n->m_UserData;
  //Obtain action declaration
  Action* a = n->m_Grist.m_Action.m_Action;
  //Get the actions function id
  Parameter* t = find_by_hash( a->m_Options, hashlittle( "id" ) );
  int fid = t ? as_integer( *t ) : a->m_Id.m_Hash;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_CONSTRUCT, ACTION_CONSTRUCT_DBGLVL );

  int err;

  // Load bss section with pointers to the data section, for the variables
  err = generate_variable_instructions( &nd->m_VD,
    n->m_Grist.m_Action.m_Parameters, p );
  if( err != 0 )
    return err;

  t = find_by_hash( a->m_Options, hashlittle( "construct" ) );
  if( t && as_bool( *t ) )
  {
    //Setup the register for the data pointer
    err = setup_variable_registry( &nd->m_VD, n->m_Grist.m_Action.m_Parameters,
      p );
    if( err != 0 )
      return err;
    if( nd->m_usesBss )
    {
      // Load bss register with bss pointer
      p->m_I.Push( INST_STORE_PB_IN_R, 1, nd->m_bssPos, 0 );
    }
    else
    {
      // Reset the bss register
      p->m_I.Push( INST__SET_REGISTRY, 1, 0, 0 );
    }
    // Load the callback id register with the correct id
    p->m_I.Push( INST__SET_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid
        & 0x0000ffff );
    // Call the construction callback
    p->m_I.Push( INST_CALL_CONS_FUN, 0, 1, 2 );
  }

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_CONSTRUCT, ACTION_CONSTRUCT_DBGLVL );
  */
  return 0;
}

int gen_exe_action( Node* n, Program* p )
{
  /*
  ActionNodeData* nd = (ActionNodeData*)n->m_UserData;
  //Obtain action declaration
  Action* a = n->m_Grist.m_Action.m_Action;
  //Get the actions function id
  Parameter* t = find_by_hash( a->m_Options, hashlittle( "id" ) );
  int fid = t ? as_integer( *t ) : a->m_Id.m_Hash;

  t = find_by_hash( a->m_Options, hashlittle( "execute" ) );
  if( !t || as_bool( *t ) )
  {
    // Enter Debug scope
    p->m_I.PushDebugScope( p, n, ACT_EXECUTE, ACTION_EXECUTE_DBGLVL );
    //Setup the register for the data pointer
    int err = setup_variable_registry( &nd->m_VD,
      n->m_Grist.m_Action.m_Parameters, p );
    if( err != 0 )
      return err;
    if( nd->m_usesBss )
    {
      // Load bss register with bss pointer
      p->m_I.Push( INST_STORE_PB_IN_R, 1, nd->m_bssPos, 0 );
    }
    else
    {
      // Reset the bss register
      p->m_I.Push( INST__SET_REGISTRY, 1, 0, 0 );
    }
    // Load the callback id register with the correct id
    p->m_I.Push( INST__SET_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid
        & 0x0000ffff );
    // Call the destruction callback
    p->m_I.Push( INST_CALL_EXEC_FUN, 0, 1, 2 );
    // Exit Debug scope
    p->m_I.PopDebugScope( p, n, ACT_EXECUTE, ACTION_EXECUTE_DBGLVL );
  }
*/
  return 0;
}

int gen_des_action( Node* n, Program* p )
{
  /*
  ActionNodeData* nd = (ActionNodeData*)n->m_UserData;
  //Obtain action declaration
  Action* a = n->m_Grist.m_Action.m_Action;
  //Get the actions function id
  Parameter* t = find_by_hash( a->m_Options, hashlittle( "id" ) );
  int fid = t ? as_integer( *t ) : a->m_Id.m_Hash;

  // Enter Debug scope
  p->m_I.PushDebugScope( p, n, ACT_DESTRUCT, ACTION_DESTRUCT_DBGLVL );

  t = find_by_hash( a->m_Options, hashlittle( "destruct" ) );
  if( t && as_bool( *t ) )
  {
    //Setup the register for the data pointer
    int err = setup_variable_registry( &nd->m_VD,
      n->m_Grist.m_Action.m_Parameters, p );
    if( err != 0 )
      return err;
    if( nd->m_usesBss )
    {
      // Load bss register with bss pointer
      p->m_I.Push( INST_STORE_PB_IN_R, 1, nd->m_bssPos, 0 );
    }
    else
    {
      // Reset the bss register
      p->m_I.Push( INST__SET_REGISTRY, 1, 0, 0 );
    }
    // Load the callback id register with the correct id
    p->m_I.Push( INST__SET_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid
        & 0x0000ffff );
    // Call the destruction callback
    p->m_I.Push( INST_CALL_DEST_FUN, 0, 1, 2 );
  }

  // Exit Debug scope
  p->m_I.PopDebugScope( p, n, ACT_DESTRUCT, ACTION_DESTRUCT_DBGLVL );
  */
  return 0;
}

int memory_need_action( Node* n )
{
  //Obtain action declaration
  Action* a = n->m_Grist.m_Action.m_Action;

  //See if the callback function needs bss space.
  Parameter* t = find_by_hash( a->m_Options, hashlittle( "bss" ) );
  int bss = t ? as_integer( *t ) : 0;

  NamedSymbol tns;
  tns.m_Type = E_ST_ACTION;
  tns.m_Symbol.m_Action = a;

  int bnv = memory_need_variables(
    n,
    n->m_Grist.m_Action.m_Parameters,
    &tns,
    tns.m_Symbol.m_Action->m_Declarations
  );
  if( bnv < 0 )
    return bnv;
  return bss + bnv;
}

/*
 *
 * Error printing functions
 *
 */

void print_missing_param_error( Node* n, Parameter* d, NamedSymbol* ns )
{
  const char* use_buff = n->m_Locator.m_Buffer;
  int use_line         = n->m_Locator.m_LineNo;
  const char* sym_t_str = "";
  const char* sym_str = "";
  Locator* ns_loc = 0x0;
  switch( ns->m_Type )
  {
  case E_ST_ACTION:
    sym_t_str = "action";
    sym_str = ns->m_Symbol.m_Action->m_Id.m_Text;
    ns_loc = &ns->m_Symbol.m_Action->m_Locator;
    break;
  case E_ST_DECORATOR:
    sym_t_str = "decorator";
    sym_str = ns->m_Symbol.m_Decorator->m_Id.m_Text;
    ns_loc = &ns->m_Symbol.m_Decorator->m_Locator;
    break;
  case E_ST_TREE:
    sym_t_str = "tree";
    sym_str = ns->m_Symbol.m_Tree->m_Id.m_Text;
    ns_loc = &ns->m_Symbol.m_Tree->m_Locator;
    break;
  case E_ST_UNKOWN:
    sym_t_str = "unknown";
    sym_str = "<unknown>";
    ns_loc = 0x0;
    break;
  }

  printf( "%s(%d) : error : parameter \"%s\" for %s \"%s\" is missing.\n",
    use_buff,
    use_line,
    d->m_Id.m_Text,
    sym_t_str,
    sym_str
  );

  if( ns_loc )
  {
    printf( "%s(%d) : error : see declaration for parameter \"%s\" for %s \"%s\".\n",
      ns_loc->m_Buffer,
      ns_loc->m_LineNo,
      d->m_Id.m_Text,
      sym_t_str,
      sym_str
    );
  }
}

void print_unable_to_convert_param_error( Node* n, Parameter* v, Parameter* d, NamedSymbol* )
{
  const char* use_buff = n->m_Locator.m_Buffer;
  int use_line         = n->m_Locator.m_LineNo;
  const char* vt_str = "";
  const char* dt_str = "";

  switch( v->m_Type )
  {
  case E_VART_UNDEFINED: vt_str = "undefined"; break;
  case E_VART_INTEGER: vt_str = "int32"; break;
  case E_VART_FLOAT: vt_str = "float"; break;
  case E_VART_STRING: vt_str = "string"; break;
  case E_VART_BOOL: vt_str = "bool"; break;
  case E_VART_HASH: vt_str = "hash"; break;
  case E_MAX_VARIABLE_TYPE: break;
  }

  switch( d->m_Type )
  {
  case E_VART_UNDEFINED: dt_str = "undefined"; break;
  case E_VART_INTEGER: dt_str = "int32"; break;
  case E_VART_FLOAT: dt_str = "float"; break;
  case E_VART_STRING: dt_str = "string"; break;
  case E_VART_BOOL: dt_str = "bool"; break;
  case E_VART_HASH: dt_str = "hash"; break;
  case E_MAX_VARIABLE_TYPE: break;
  }

  printf( "%s(%d) : error : parameter \"%s\" can't be converted from type %s to %s.\n",
    use_buff,
    use_line,
    d->m_Id.m_Text,
    vt_str,
    dt_str
  );
}

/*
 *
 * Argument list functions
 *
 */

int memory_need_variables( Node* vars_n, Parameter* vars, NamedSymbol* dec_s, Parameter* dec )
{
  if( !vars && !dec )
    return 0;

  if( !dec )
    return 0;

  if( !vars )
  {
    Parameter* it;
    for( it = dec; it != 0x0; it = it->m_Next )
      print_missing_param_error( vars_n, it, dec_s );
    return -1;
  }
  return sizeof( void* ) * count_elements( dec );
}

int store_variables_in_data_section(
    VariableGenerateData* vd,
    Node* vars_n,
    Parameter* vars,
    NamedSymbol* dec_s,
    Parameter* dec,
    Program* p,
    int mo
  )
{
/*
  vd->m_Data.clear();
  vd->m_bssStart = 0;

  if( !vars && !dec )
    return mo;

  if( !dec )
    return mo;

  if( !vars )
  {
    Parameter* it;
    for( it = dec; it != 0x0; it = it->m_Next )
      print_missing_param_error( vars_n, it, dec_s );
    return -1;
  }

  vd->m_bssStart = mo;
  mo += sizeof(void*) * count_elements( dec );

  bool errors = false;
  Parameter* it;
  for( it = dec; it != 0x0; it = it->m_Next )
  {
    Parameter* v = find_by_hash( vars, it->m_Id.m_Hash );
    if( v && safe_to_convert( v, it->m_Type ) )
      continue;

    errors = true;

    if( !v )
      print_missing_param_error( vars_n, it, dec_s );
    else
      print_unable_to_convert_param_error( vars_n, v, it, dec_s );
  }

  if( errors )
  {
    vd->m_bssStart = 0;
    vd->m_Data.clear();
    return -1;
  }

  DataSection& d = p->m_D;
  for( it = dec; it != 0x0; it = it->m_Next )
  {
    Parameter* v = find_by_hash( vars, it->m_Id.m_Hash );
    switch( it->m_Type )
    {
    case E_VART_INTEGER:
      vd->m_Data.push_back( d.PushInteger( as_integer( *v ) ) );
      break;
    case E_VART_FLOAT:
      vd->m_Data.push_back( d.PushFloat( as_float( *v ) ) );
      break;
    case E_VART_STRING:
      vd->m_Data.push_back( d.PushString( as_string( *v )->m_Parsed ) );
      break;
    case E_VART_BOOL:
      vd->m_Data.push_back( d.PushInteger( as_integer( *v ) ) );
      break;
    case E_VART_HASH:
      vd->m_Data.push_back( d.PushInteger( as_hash( *v ) ) );
      break;
    case E_VART_UNDEFINED:
    case E_MAX_VARIABLE_TYPE:
      return -1;
      break;
    }
  }
  */
  return mo;
}

int generate_variable_instructions( VariableGenerateData* vd, Parameter*,
  Program* p )
{
  /*
  IntVector::iterator it, it_e( vd->m_Data.end() );
  int i = 0;
  for( it = vd->m_Data.begin(); it != it_e; ++it, ++i )
  {
    //Store a pointer to a variable in the data section in the bss section.
    p->m_I.Push( INST_STORE_PD_IN_B, vd->m_bssStart + (sizeof(void*) * i), *it,
      0 );
  }
  */
  return 0;
}

int setup_variable_registry( VariableGenerateData* vd, Parameter*,
  Program* p )
{
  /*
  if( !vd->m_Data.empty() )
  {
    // Load the user data register with a pointer to the bss structure for variables
    p->m_I.Push( INST_STORE_PB_IN_R, 2, vd->m_bssStart, 0 );
  }
  else
  {
    //Load the user data register with a null pointer
    p->m_I.Push( INST__SET_REGISTRY, 2, 0, 0 );
  }
  */
  return 0;
}


void patch_calls( Node* n, Program* p )
{
  Node* c = get_first_child( n );
  while( c )
  {
    patch_calls( c, p );
    c = c->m_Next;
  }
  if( n->m_Grist.m_Type == E_GRIST_TREE )
  {
    TreeNodeData* nd = (TreeNodeData*)n->m_UserData;
    BehaviorTreeList* btl = p->m_First;
    while( btl )
    {
      if( btl->m_Tree == n->m_Grist.m_Tree.m_Tree )
      {
        p->m_I.SetA1( nd->m_CallPatch, btl->m_FirstInst );
        p->m_I.SetA1( nd->m_DestructPatch, btl->m_FirstInst );
      }
      btl = btl->m_Next;
    }
  }
}

