/*******************************************************************************
 * Copyright (c) 2010-02-22 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include "saver.h"
#include "../btree_internal.h"

#include <btree/btree_func.h>

#include <stdio.h>

void SaveIncludes( SaverContext );
void SaveActions( SaverContext );
void SaveDecorators( SaverContext );
void SaveTrees( SaverContext );
void SaveVariableList( SaverContext, Variable* );
void SaveNodeList( SaverContext, Node*, int );
void FlushBuffer( SaverContext );

#define SAVE_BUFFER_FLUSH_LIMIT (7 * 1024)

void* SaverContextGetExtra( SaverContext sc )
{
  return sc->m_Extra;
}

void SaverContextSetExtra( SaverContext sc, void* extra )
{
  sc->m_Extra = extra;
}

BehaviorTreeContext ParserContextGetBehaviorTreeContext( SaverContext sc )
{
  return sc->m_Tree;
}

void Save( SaverContext sc, SaverContextFunctions* funcs )
{
  sc->m_Funcs = *funcs;
  StringBufferAppend( &sc->m_Buffer, "\n; Includes\n\n");
  SaveIncludes( sc );
  StringBufferAppend( &sc->m_Buffer, "; Actions\n\n" );
  SaveActions( sc );
  StringBufferAppend( &sc->m_Buffer, "; Decorators\n\n" );
  SaveDecorators( sc );
  StringBufferAppend( &sc->m_Buffer, "; Trees\n\n" );
  SaveTrees( sc );
  FlushBuffer( sc );
}

void SaveIncludes( SaverContext sc )
{
  Include* i = sc->m_Tree->m_Includes;
  while( i )
  {
    const char* translated = sc->m_Funcs.m_Translate( sc, i->m_Name );
    StringBufferAppend( &sc->m_Buffer, "(include \"" );
    StringBufferAppend( &sc->m_Buffer, translated );
    StringBufferAppend( &sc->m_Buffer, "\")\n" );

    if( sc->m_Buffer.m_Size >= SAVE_BUFFER_FLUSH_LIMIT )
      FlushBuffer( sc );

    i = i->m_Next;
  }
  StringBufferAppend( &sc->m_Buffer, '\n' );
}

void SaveActions( SaverContext sc )
{
  int count;
  NamedSymbol* s = BehaviorTreeContextAccessSymbols( sc->m_Tree, &count );
  for( int i = 0; i < count; ++i )
  {
    if( s[i].m_Type != E_ST_ACTION )
      continue;
    if( !s[i].m_Symbol.m_Action->m_Declared )
      continue;
    Action* a = s[i].m_Symbol.m_Action;
    StringBufferAppend( &sc->m_Buffer, "(defact " );
    StringBufferAppend( &sc->m_Buffer, a->m_Id.m_Text );
    StringBufferAppend( &sc->m_Buffer, "\n " );

    SaveVariableList( sc, a->m_Options );
    StringBufferAppend( &sc->m_Buffer, "\n " );

    SaveVariableList( sc, a->m_Args );
    StringBufferAppend( &sc->m_Buffer, '\n' );

    StringBufferAppend( &sc->m_Buffer, ")\n" );

    if( sc->m_Buffer.m_Size >= SAVE_BUFFER_FLUSH_LIMIT )
      FlushBuffer( sc );
  }
  StringBufferAppend( &sc->m_Buffer, '\n' );
}

void SaveDecorators( SaverContext sc )
{
  int count;
  NamedSymbol* s = BehaviorTreeContextAccessSymbols( sc->m_Tree, &count );
  for( int i = 0; i < count; ++i )
  {
    if( s[i].m_Type != E_ST_DECORATOR )
      continue;
    if( !s[i].m_Symbol.m_Decorator->m_Declared )
      continue;
    Decorator* d = s[i].m_Symbol.m_Decorator;
    StringBufferAppend( &sc->m_Buffer, "(defdec " );
    StringBufferAppend( &sc->m_Buffer, d->m_Id.m_Text );
    StringBufferAppend( &sc->m_Buffer, "\n " );

    SaveVariableList( sc, d->m_Options );
    StringBufferAppend( &sc->m_Buffer, "\n " );

    SaveVariableList( sc, d->m_Args );
    StringBufferAppend( &sc->m_Buffer, '\n' );

    StringBufferAppend( &sc->m_Buffer, ")\n" );

    if( sc->m_Buffer.m_Size >= SAVE_BUFFER_FLUSH_LIMIT )
      FlushBuffer( sc );
  }
  StringBufferAppend( &sc->m_Buffer, '\n' );
}

void SaveTrees( SaverContext sc )
{
  int count;
  NamedSymbol* s = BehaviorTreeContextAccessSymbols( sc->m_Tree, &count );
  for( int i = 0; i < count; ++i )
  {
    if( s[i].m_Type != E_ST_TREE )
      continue;
    if( !s[i].m_Symbol.m_Tree->m_Declared )
      continue;
    BehaviorTree* t = s[i].m_Symbol.m_Tree;
    StringBufferAppend( &sc->m_Buffer, "(deftree " );
    StringBufferAppend( &sc->m_Buffer, t->m_Id.m_Text );
    StringBufferAppend( &sc->m_Buffer, ' ' );
    SaveNodeList( sc, t->m_Root, 1 );
    StringBufferAppend( &sc->m_Buffer, ")\n\n" );


    if( sc->m_Buffer.m_Size >= SAVE_BUFFER_FLUSH_LIMIT )
      FlushBuffer( sc );
  }
}

void SaveVariableDeclaration( SaverContext sc, Variable* v )
{
  switch( v->m_Type )
  {
  case E_VART_INTEGER:
    StringBufferAppend( &sc->m_Buffer, "int32 " );
    StringBufferAppend( &sc->m_Buffer, v->m_Id.m_Text );
    break;
  case E_VART_FLOAT:
    StringBufferAppend( &sc->m_Buffer, "float " );
    StringBufferAppend( &sc->m_Buffer, v->m_Id.m_Text );
    break;
  case E_VART_STRING:
    StringBufferAppend( &sc->m_Buffer, "string " );
    StringBufferAppend( &sc->m_Buffer, v->m_Id.m_Text );
    break;
  case E_VART_BOOL:
    StringBufferAppend( &sc->m_Buffer, "bool " );
    StringBufferAppend( &sc->m_Buffer, v->m_Id.m_Text );
    break;
  case E_VART_UNDEFINED:
  case E_MAX_VARIABLE_TYPE:
    break;
  }
}

void SaveVariableAssignment( SaverContext sc, Variable* v )
{
  char tmp[128];
  switch( v->m_Type )
  {
  case E_VART_INTEGER:
    StringBufferAppend( &sc->m_Buffer, v->m_Id.m_Text );
    StringBufferAppend( &sc->m_Buffer, ' ' );
    sprintf( tmp, "%d", ValueAsInteger( *v ) );
    StringBufferAppend( &sc->m_Buffer, tmp );
    break;
  case E_VART_FLOAT:
    StringBufferAppend( &sc->m_Buffer, v->m_Id.m_Text );
    StringBufferAppend( &sc->m_Buffer, ' ' );
    sprintf( tmp, "%f", ValueAsFloat( *v ) );
    StringBufferAppend( &sc->m_Buffer, tmp );
    break;
  case E_VART_STRING:
    StringBufferAppend( &sc->m_Buffer, v->m_Id.m_Text );
    StringBufferAppend( &sc->m_Buffer, " \"" );
    StringBufferAppend( &sc->m_Buffer, ValueAsString( *v )->m_Raw );
    StringBufferAppend( &sc->m_Buffer, '\"' );
    break;
  case E_VART_BOOL:
    StringBufferAppend( &sc->m_Buffer, v->m_Id.m_Text );
    StringBufferAppend( &sc->m_Buffer, ' ' );
    StringBufferAppend( &sc->m_Buffer, ValueAsBool( *v ) ? "true" : "false" );
    break;
  case E_VART_UNDEFINED:
  case E_MAX_VARIABLE_TYPE:
    break;
  }
}

void SaveVariableList( SaverContext sc, Variable* v )
{
  if( !v )
  {
    StringBufferAppend( &sc->m_Buffer, "'()" );
    return;
  }

  StringBufferAppend( &sc->m_Buffer, "'(" );

  while( v )
  {
    StringBufferAppend( &sc->m_Buffer, '(' );
    if( v->m_ValueSet )
      SaveVariableAssignment( sc, v );
    else
      SaveVariableDeclaration( sc, v );
    StringBufferAppend( &sc->m_Buffer, ')' );
    v = v->m_Next;
    if( v )
      StringBufferAppend( &sc->m_Buffer, ' ' );

    if( sc->m_Buffer.m_Size >= SAVE_BUFFER_FLUSH_LIMIT )
      FlushBuffer( sc );
  }
  StringBufferAppend( &sc->m_Buffer, ')' );
}

void SaveNode( SaverContext, Node*, int );

void AppendDepth( SaverContext sc, int depth )
{
  if( depth == 0 )
    return;
  char depth_buff[ 128 ];
  char space_buff[ 1024 ];
  sprintf( depth_buff, "%%-%ds", depth );
  sprintf( space_buff, depth_buff, "" );
  StringBufferAppend( &sc->m_Buffer, space_buff );
}

void SaveSequence( SaverContext sc, Node* n, int depth )
{
  AppendDepth( sc, depth );
  StringBufferAppend( &sc->m_Buffer, "(sequence " );
  SaveNodeList( sc, GetFirstChild( n ), depth + 1 );
  StringBufferAppend( &sc->m_Buffer, ")\n" );
}

void SaveSelector( SaverContext sc, Node* n, int depth )
{
  AppendDepth( sc, depth );
  StringBufferAppend( &sc->m_Buffer, "(selector " );
  SaveNodeList( sc, GetFirstChild( n ), depth + 1 );
  StringBufferAppend( &sc->m_Buffer, ")\n" );
}

void SaveParallel( SaverContext sc, Node* n, int depth )
{
  AppendDepth( sc, depth );
  StringBufferAppend( &sc->m_Buffer, "(parallel " );
  SaveNodeList( sc, GetFirstChild( n ), depth + 1 );
  StringBufferAppend( &sc->m_Buffer, ")\n" );
}

void SaveDynSelector( SaverContext sc, Node* n, int depth )
{
  AppendDepth( sc, depth );
  StringBufferAppend( &sc->m_Buffer, "(dyn_selector " );
  SaveNodeList( sc, GetFirstChild( n ), depth + 1 );
  StringBufferAppend( &sc->m_Buffer, ")\n" );
}

void SaveSucceed( SaverContext sc, Node* n, int depth )
{
  StringBufferAppend( &sc->m_Buffer, "(succeed)" );
}
void SaveFail( SaverContext sc, Node* n, int depth )
{
  StringBufferAppend( &sc->m_Buffer, "(fail)" );
}
void SaveWork( SaverContext sc, Node* n, int depth )
{
  StringBufferAppend( &sc->m_Buffer, "(work)" );
}

void SaveDecorator( SaverContext sc, Node* n, int depth )
{
  AppendDepth( sc, depth );
  StringBufferAppend( &sc->m_Buffer, "(decorator '" );
  StringBufferAppend( &sc->m_Buffer, n->m_Grist.m_Decorator.m_Decorator->m_Id.m_Text );
  StringBufferAppend( &sc->m_Buffer, ' ' );
  SaveVariableList( sc, n->m_Grist.m_Decorator.m_Arguments );
  StringBufferAppend( &sc->m_Buffer, '\n' );
  SaveNode( sc, n->m_Grist.m_Decorator.m_Child, depth + 1 );
  AppendDepth( sc, depth );
  StringBufferAppend( &sc->m_Buffer, ")\n" );
}

void SaveAction( SaverContext sc, Node* n, int depth )
{
  AppendDepth( sc, depth );
  StringBufferAppend( &sc->m_Buffer, "(action '" );
  StringBufferAppend( &sc->m_Buffer, n->m_Grist.m_Action.m_Action->m_Id.m_Text );
  StringBufferAppend( &sc->m_Buffer, ' ' );
  SaveVariableList( sc, n->m_Grist.m_Action.m_Arguments );
  StringBufferAppend( &sc->m_Buffer, ")\n" );
}

void SaveNode( SaverContext sc, Node* n, int depth )
{
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_SEQUENCE:
    SaveSequence( sc, n, depth );
    break;
  case E_GRIST_SELECTOR:
    SaveSelector( sc, n, depth );
    break;
  case E_GRIST_PARALLEL:
    SaveParallel( sc, n, depth );
    break;
  case E_GRIST_DYN_SELECTOR:
    SaveDynSelector( sc, n, depth );
    break;
  case E_GRIST_SUCCEED:
    SaveSucceed( sc, n, depth );
    break;
  case E_GRIST_FAIL:
    SaveFail( sc, n, depth );
    break;
  case E_GRIST_WORK:
    SaveWork( sc, n, depth );
    break;
  case E_GRIST_DECORATOR:
    SaveDecorator( sc, n, depth );
    break;
  case E_GRIST_ACTION:
    SaveAction( sc, n, depth );
    break;
  case E_GRIST_UNKOWN:
  case E_MAX_GRIST_TYPES:
    /* Warning killer */
    break;
  }
  if( sc->m_Buffer.m_Size >= SAVE_BUFFER_FLUSH_LIMIT )
    FlushBuffer( sc );
}

void SaveNodeList( SaverContext sc, Node* n, int depth )
{
  StringBufferAppend( &sc->m_Buffer, "'(" );
  if( n )
    StringBufferAppend( &sc->m_Buffer, '\n' );

  while( n )
  {
    SaveNode( sc, n, depth );
    n = n->m_Next;
  }
  AppendDepth( sc, depth - 1 );
  StringBufferAppend( &sc->m_Buffer, ")" );
}

void FlushBuffer( SaverContext sc )
{
  sc->m_Funcs.m_Write( sc, sc->m_Buffer.m_Str, sc->m_Buffer.m_Size );
  StringBufferClear( &sc->m_Buffer );
}

#undef SAVE_BUFFER_FLUSH_LIMIT
