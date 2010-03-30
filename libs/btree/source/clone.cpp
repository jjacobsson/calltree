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

#include "btree_internal.h"

#include <btree/btree_data.h>
#include <btree/btree_func.h>



BehaviorTreeContext clone( BehaviorTreeContext obtc )
{
  ObjectPoolSetup ops;
  ops = obtc->m_Pool->m_Setup;
  ObjectPool* op = create_object_pool( &ops );
  SBehaviorTreeContext* btc =
      &(((ObjectFootPrint*)allocate_object( op ))->m_BTContext);
  btc->m_Pool = op;
  btc->m_Allocator = obtc->m_Allocator;
  btc->m_Includes = 0x0;

  init( &btc->m_StringTable, btc->m_Allocator );
  init( &btc->m_SymbolTable, btc->m_Allocator );

  clone( btc, obtc->m_Includes );
  btc->m_Options = clone_list( btc, obtc->m_Options );

  int count;
  NamedSymbol* ns =  access_symbols( obtc, &count );
  clone_symbols( btc, ns, count );

  return btc;
}

/*
 * Cloning functions
 */

void clone( BehaviorTreeContext btc, Include* inc )
{
  while( inc )
  {
    create_include( btc, inc->m_Name );
    inc = inc->m_Next;
  }
}

void clone_symbols( BehaviorTreeContext btc, NamedSymbol* ns, int count )
{
  for( int i = 0; i < count; ++i )
  {
    switch( ns[i].m_Type )
    {
    case E_ST_UNKOWN:
    case E_ST_TREE:
      clone( btc, ns[i].m_Symbol.m_Tree );
      break;
    case E_ST_ACTION:
      clone( btc, ns[i].m_Symbol.m_Action );
      break;
    case E_ST_DECORATOR:
      clone( btc, ns[i].m_Symbol.m_Decorator );
      break;
    case E_MAX_SYMBOL_TYPES:
      break;
    }
  }
}

void clone( BehaviorTreeContext btc, Identifier* d, Identifier* s )
{
  *d = *s;
  d->m_Text = register_string( btc, s->m_Text, s->m_Hash );
}

void clone( BehaviorTreeContext btc, BehaviorTree* tree )
{
  BehaviorTree* t = look_up_behavior_tree( btc, &tree->m_Id );
  t->m_Declared = tree->m_Declared;
  t->m_Root = clone_list( btc, tree->m_Root );
}

void clone( BehaviorTreeContext btc, Action* src )
{
  Action* a = look_up_action( btc, &src->m_Id );
  a->m_Declarations = clone_list( btc, src->m_Declarations );
  a->m_Options = clone_list( btc, src->m_Options );
  a->m_Declared = src->m_Declared;
}

void clone( BehaviorTreeContext btc, Decorator* src )
{
  Decorator* d = look_up_decorator( btc, &src->m_Id );
  d->m_Declarations = clone_list( btc, src->m_Declarations );
  d->m_Options = clone_list( btc, src->m_Options );
  d->m_Declared = src->m_Declared;
}

Parameter* clone_list( BehaviorTreeContext btc, Parameter* v1 )
{

  Parameter* v2 = 0x0;
  Parameter* r = 0x0;
  while( v1 )
  {
    Parameter* v = (Parameter*)allocate_object( btc );
    init( v );

    clone( btc, &v->m_Id, &v1->m_Id );
    v->m_Data = v1->m_Data;
    v->m_Type = v1->m_Type;
    v->m_ValueSet = v1->m_ValueSet;

    if( v->m_ValueSet )
    {
      switch( v->m_Type )
      {
      case E_VART_UNDEFINED:
      case E_VART_INTEGER:
      case E_VART_FLOAT:
        break;
      case E_VART_STRING:
        v->m_Data.m_String.m_Parsed = register_string( btc, v1->m_Data.m_String.m_Parsed );
        v->m_Data.m_String.m_Raw = register_string( btc, v1->m_Data.m_String.m_Raw );
        break;
      case E_VART_BOOL:
      case E_VART_HASH:
      case E_MAX_VARIABLE_TYPE:
        break;
      }
    }

    if( v2 )
      v2->m_Next = v;
    else
      r = v;

    v2 = v;
    v1 = v1->m_Next;
  }
  return r;
}

void clone_decorator_node( BehaviorTreeContext btc, Node* dest, Node* src )
{
  dest->m_Grist.m_Decorator.m_Parameters = clone_list( btc, src->m_Grist.m_Decorator.m_Parameters );
  if( src->m_Grist.m_Decorator.m_Decorator )
    dest->m_Grist.m_Decorator.m_Decorator = look_up_decorator( btc, &src->m_Grist.m_Decorator.m_Decorator->m_Id );
}

void clone_action_node( BehaviorTreeContext btc, Node* dest, Node* src )
{
  dest->m_Grist.m_Action.m_Parameters = clone_list( btc, src->m_Grist.m_Action.m_Parameters );
  if( src->m_Grist.m_Action.m_Action )
    dest->m_Grist.m_Action.m_Action = look_up_action( btc, &src->m_Grist.m_Action.m_Action->m_Id );
}

void clone_tree_node( BehaviorTreeContext btc, Node* dest, Node* src )
{
  if( src->m_Grist.m_Tree.m_Tree )
    dest->m_Grist.m_Tree.m_Tree = look_up_behavior_tree( btc, &src->m_Grist.m_Tree.m_Tree->m_Id );
}

Node* clone_list( BehaviorTreeContext btc, Node* n )
{
  if( !n )
    return 0x0;

  Node* r = (Node*)allocate_object( btc );
  init( r );

  *r = *n;

  r->m_Next = 0x0;
  r->m_Prev = 0x0;
  r->m_UserData = 0x0;

  switch( r->m_Grist.m_Type )
  {
  case E_GRIST_DECORATOR:
    clone_decorator_node( btc, r, n );
    break;
  case E_GRIST_ACTION:
    clone_action_node( btc, r, n );
    break;
  case E_GRIST_TREE:
    clone_tree_node( btc, r, n );
    break;
  case E_GRIST_UNKOWN:
  case E_GRIST_SEQUENCE:
  case E_GRIST_SELECTOR:
  case E_GRIST_PARALLEL:
  case E_GRIST_DYN_SELECTOR:
  case E_GRIST_SUCCEED:
  case E_GRIST_FAIL:
  case E_GRIST_WORK:
  case E_MAX_GRIST_TYPES:
    /* Warning killers */
    break;
  }

  set_first_child( r, clone_list( btc, get_first_child( n ) ) );
  set_parent_on_children( r );
  r->m_Next = clone_list( btc, n->m_Next );
  if( r->m_Next )
    r->m_Next->m_Prev = r;

  return r;
}
