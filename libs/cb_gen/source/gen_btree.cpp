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

void gen_node_con( Function* f, Node* n )
{

}

}


