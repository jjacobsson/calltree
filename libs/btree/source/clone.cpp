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

void CloneIncludes( BehaviorTreeContext, Include* );
void CloneDeclarations( BehaviorTreeContext, NamedSymbol*, int );
void CloneId( BehaviorTreeContext, Identifier*, Identifier* );
void CloneTree( BehaviorTreeContext, BehaviorTree* );
void CloneAction( BehaviorTreeContext, Action* );
void CloneDecorator( BehaviorTreeContext, Decorator* );
Node* CloneNodeList( BehaviorTreeContext, Node* );

BehaviorTreeContext BehaviorTreeContextClone( BehaviorTreeContext obtc )
{
  ObjectPoolSetup ops;
  ops = obtc->m_Pool->m_Setup;
  ObjectPool* op = CreateObjectPool( &ops );
  SBehaviorTreeContext* btc =
      &(((ObjectFootPrint*)AllocateObject( op ))->m_BTContext);
  btc->m_Pool = op;
  btc->m_Allocator = obtc->m_Allocator;
  btc->m_Includes = 0x0;

  StringTableInit( &btc->m_StringTable, btc->m_Allocator );
  SymbolTableInit( &btc->m_SymbolTable, btc->m_Allocator );

  CloneIncludes( btc, obtc->m_Includes );

  int count;
  NamedSymbol* ns =  BehaviorTreeContextAccessSymbols( obtc, &count );
  CloneDeclarations( btc, ns, count );

  return btc;
}

/*
 * Cloning functions
 */

void CloneIncludes( BehaviorTreeContext btc, Include* inc )
{
  while( inc )
  {
    BehaviorTreeContextCreateInclude( btc, inc->m_Name );
    inc = inc->m_Next;
  }
}

void CloneDeclarations( BehaviorTreeContext btc, NamedSymbol* ns, int count )
{
  for( int i = 0; i < count; ++i )
  {
    switch( ns[i].m_Type )
    {
    case E_ST_UNKOWN:
    case E_ST_TREE:
      break;
    case E_ST_ACTION:
      CloneAction( btc, ns[i].m_Symbol.m_Action );
      break;
    case E_ST_DECORATOR:
      CloneDecorator( btc, ns[i].m_Symbol.m_Decorator );
      break;
    case E_MAX_SYMBOL_TYPES:
      break;
    }
  }
  for( int i = 0; i < count; ++i )
  {
    switch( ns[i].m_Type )
    {
    case E_ST_UNKOWN:
      break;
    case E_ST_TREE:
      CloneTree( btc, ns[i].m_Symbol.m_Tree );
      break;
    case E_ST_ACTION:
    case E_ST_DECORATOR:
    case E_MAX_SYMBOL_TYPES:
      break;
    }
  }
}

void CloneId( BehaviorTreeContext btc, Identifier* d, Identifier* s )
{
  *d = *s;
  d->m_Text = BehaviorTreeContextRegisterString( btc, s->m_Text, s->m_Hash );
}

void CloneTree( BehaviorTreeContext btc, BehaviorTree* tree )
{
  NamedSymbol* ns = BehaviorTreeContextFindSymbol( btc, tree->m_Id.m_Hash );
  BehaviorTree* new_tree = 0x0;
  if( ns )
    new_tree = ns->m_Symbol.m_Tree;
  else
  {
    new_tree = (BehaviorTree*)BehaviorTreeContextAllocateObject( btc );
    InitBehaviorTree( new_tree );
  }
  new_tree->m_Declared = tree->m_Declared;
  CloneId( btc, &new_tree->m_Id, &tree->m_Id );

  if( ns )
    ns->m_Symbol.m_Tree = new_tree;
  else
  {
    NamedSymbol nsym;
    nsym.m_Type = E_ST_TREE;
    nsym.m_Symbol.m_Tree = new_tree;
    BehaviorTreeContextRegisterSymbol( btc, nsym );
  }
  new_tree->m_Root = CloneNodeList( btc, tree->m_Root );
  Node* n = new_tree->m_Root;
  while( n )
  {
    n->m_Pare.m_Type = E_NP_TREE;
    n->m_Pare.m_Tree = new_tree;
    n = n->m_Next;
  }
}

void CloneAction( BehaviorTreeContext btc, Action* src )
{
  Action* dest = (Action*)BehaviorTreeContextAllocateObject( btc );
  InitAction( dest );
  CloneId( btc, &dest->m_Id, &src->m_Id );
  dest->m_Args = CloneVariableList( btc, src->m_Args );
  dest->m_Options = CloneVariableList( btc, src->m_Options );
  dest->m_Declared = src->m_Declared;

  NamedSymbol ns;
  ns.m_Type = E_ST_ACTION;
  ns.m_Symbol.m_Action = dest;
  BehaviorTreeContextRegisterSymbol( btc, ns );
}

void CloneDecorator( BehaviorTreeContext btc, Decorator* src )
{
  Decorator* dest = (Decorator*)BehaviorTreeContextAllocateObject( btc );
  InitDecorator( dest );
  CloneId( btc, &dest->m_Id, &src->m_Id );
  dest->m_Args = CloneVariableList( btc, src->m_Args );
  dest->m_Options = CloneVariableList( btc, src->m_Options );
  dest->m_Declared = src->m_Declared;

  NamedSymbol ns;
  ns.m_Type = E_ST_DECORATOR;
  ns.m_Symbol.m_Decorator = dest;
  BehaviorTreeContextRegisterSymbol( btc, ns );
}

Variable* CloneVariableList( BehaviorTreeContext btc, Variable* v1 )
{

  Variable* v2 = 0x0;
  Variable* r = 0x0;
  while( v1 )
  {
    Variable* v = (Variable*)BehaviorTreeContextAllocateObject( btc );
    InitVariable( v );

    CloneId( btc, &v->m_Id, &v1->m_Id );
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
        v->m_Data.m_String.m_Parsed = BehaviorTreeContextRegisterString( btc, v1->m_Data.m_String.m_Parsed );
        v->m_Data.m_String.m_Raw = BehaviorTreeContextRegisterString( btc, v1->m_Data.m_String.m_Raw );
        break;
      case E_VART_BOOL:
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

void CloneDecoratorNode( BehaviorTreeContext btc, Node* dest, Node* src )
{
  dest->m_Grist.m_Decorator.m_Arguments = CloneVariableList( btc, src->m_Grist.m_Decorator.m_Arguments );
  if( src->m_Grist.m_Decorator.m_Decorator )
  {
    NamedSymbol* ns = BehaviorTreeContextFindSymbol( btc, src->m_Grist.m_Decorator.m_Decorator->m_Id.m_Hash );
    if( ns )
      dest->m_Grist.m_Decorator.m_Decorator = ns->m_Symbol.m_Decorator;
  }
}

void CloneActionNode( BehaviorTreeContext btc, Node* dest, Node* src )
{
  dest->m_Grist.m_Action.m_Arguments = CloneVariableList( btc, src->m_Grist.m_Action.m_Arguments );
  if( src->m_Grist.m_Action.m_Action )
  {
    NamedSymbol* ns = BehaviorTreeContextFindSymbol( btc, src->m_Grist.m_Action.m_Action->m_Id.m_Hash );
    if( ns )
      dest->m_Grist.m_Action.m_Action = ns->m_Symbol.m_Action;
  }
}

Node* CloneNodeList( BehaviorTreeContext btc, Node* n )
{
  if( !n )
    return 0x0;

  Node* r = (Node*)BehaviorTreeContextAllocateObject( btc );
  InitNode( r );

  *r = *n;

  r->m_Next = 0x0;
  r->m_Prev = 0x0;
  r->m_UserData = 0x0;

  switch( r->m_Grist.m_Type )
  {
  case E_GRIST_DECORATOR:
    CloneDecoratorNode( btc, r, n );
    break;
  case E_GRIST_ACTION:
    CloneActionNode( btc, r, n );
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

  SetFirstChild( r, CloneNodeList( btc, GetFirstChild( n ) ) );
  SetParentOnChildren( r );
  r->m_Next = CloneNodeList( btc, n->m_Next );
  if( r->m_Next )
    r->m_Next->m_Prev = r;

  return r;
}
