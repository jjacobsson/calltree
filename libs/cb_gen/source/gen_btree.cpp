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

#include "gen_btree.h"

namespace cb_gen {

int sequence_memory_needed( Node* n );
void gen_sequence_con( Function*, Node*, uint );
void gen_sequence_exe( Function*, Node*, uint );
void gen_sequence_des( Function*, Node*, uint );

void create_functions( Node* n, Program* p );
void create_functions( BehaviorTree* t, Program* p );

void create_functions( Node* n, Program* p )
{
  if( !n )
    return;

  if( n->m_Grist.m_Type == E_GRIST_TREE )
    create_functions( n->m_Grist.m_Tree.m_Tree, p );

  Node* c = get_first_child( n );
  while( c )
  {
    create_functions( c, p );
    c = c->m_Next;
  }
}

void create_functions( BehaviorTree* t, Program* p )
{
  if( !t )
    return;

  uint fidx = find_function( p, t->m_Id.m_Hash );
  if( fidx != ~0 )
    return;

  Function* f = new Function;
  init( f );
  f->m_T = t;
  f->m_P = p;

  t->m_UserData = f;

  FunctionEntry fe;
  fe.m_F  = f;
  fe.m_Id = t->m_Id.m_Hash;
  p->m_F.push_back( fe );

  Node* n = t->m_Root;
  while( n )
  {
    create_functions( n, p );
    n = n->m_Next;
  }
}

void create_functions( BehaviorTreeContext ctx, Program* p )
{
  NamedSymbol* ns = find_symbol( ctx, "main" );
  //TODO: Error report
  if( ns->m_Type != E_ST_TREE )
    return;

  create_functions( ns->m_Symbol.m_Tree, p );
}

void gen_node_con( Function* f, Node* n, uint prio )
{
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_SEQUENCE:
    gen_sequence_con( f, n, prio );
    break;
  }
}
void gen_node_exe( Function* f, Node* n, uint prio )
{
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_SEQUENCE:
    gen_sequence_exe( f, n, prio );
    break;
  }
}
void gen_node_des( Function* f, Node* n, uint prio )
{
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_SEQUENCE:
    gen_sequence_des( f, n, prio );
    break;
  }
}



int selector_memory_needed( Node* n )
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

int parallel_memory_needed( Node* n )
{
  Node* c = get_first_child( n );
  if( !c )
    return 0;

  int mem = sizeof( unsigned int ) * 2;
  while( c )
  {
    mem += memory_needed( c );
    c = c->m_Next;
  }
  return mem;
}

int dyn_selector_memory_needed( Node* n )
{
  Node* c = get_first_child( n );
  if( !c )
    return 0;

  int mem = sizeof( unsigned int ) * 2;
  while( c )
  {
    mem += memory_needed( c );
    c = c->m_Next;
  }
  return mem;
}

int tree_memory_needed( Node* n )
{
  Function* f = (Function*)n->m_Grist.m_Tree.m_Tree->m_UserData;
  count_memory( f );
  return f->m_Memory;
}

int action_memory_needed( Node* n )
{
  return
    sizeof( cb::CallData ) +
    sizeof( unsigned int ) * count_elements( get_parameters( n ) );
}

int decorator_memory_needed( Node* n )
{
  Node* c = get_first_child( n );
  if(!c)
    return 0;

  return
    sizeof( cb::CallData ) +
    sizeof( unsigned int ) * count_elements( get_parameters( n ) ) +
    memory_needed( c );
}

int memory_needed( BehaviorTree* t )
{
  int max = 0;
  Node* c = t->m_Root;

  while( c )
  {
    int t = memory_needed( c );
    if( t > max )
      max = t;
    c = c->m_Next;
  }
  return max;
}

int memory_needed( Node* n )
{
  int r = 0xffffffff;
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_UNKOWN:
    /* Warning killer */
    break;
  case E_GRIST_SEQUENCE:
    r = sequence_memory_needed( n );
    break;
  case E_GRIST_SELECTOR:
    r = selector_memory_needed( n );
    break;
  case E_GRIST_PARALLEL:
    r = parallel_memory_needed( n );
    break;
  case E_GRIST_DYN_SELECTOR:
    r = dyn_selector_memory_needed( n );
    break;
  case E_GRIST_SUCCEED:
    r = 0;
    break;
  case E_GRIST_FAIL:
    r = 0;
    break;
  case E_GRIST_WORK:
    r = 0;
    break;
  case E_GRIST_TREE:
    r = tree_memory_needed( n );
    break;
  case E_GRIST_ACTION:
    r = action_memory_needed( n );
    break;
  case E_GRIST_DECORATOR:
    r = decorator_memory_needed( n );
    break;
  case E_MAX_GRIST_TYPES:
    /* Warning killer */
    break;
  }
  return r;
}

}


