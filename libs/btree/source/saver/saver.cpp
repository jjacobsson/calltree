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
  SaveIncludes( sc );
  SaveActions( sc );
  SaveDecorators( sc );
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
    if( a->m_Options )
    {
      SaveVariableList( sc, a->m_Options );
      StringBufferAppend( &sc->m_Buffer, "\n " );
    }
    else
      StringBufferAppend( &sc->m_Buffer, "'()\n " );

    if( a->m_Args )
    {
      SaveVariableList( sc, a->m_Args );
      StringBufferAppend( &sc->m_Buffer, '\n' );
    }
    else
      StringBufferAppend( &sc->m_Buffer, "'()\n" );

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
    if( d->m_Options )
    {
      SaveVariableList( sc, d->m_Options );
      StringBufferAppend( &sc->m_Buffer, "\n " );
    }
    else
      StringBufferAppend( &sc->m_Buffer, "'()\n " );

    if( d->m_Args )
    {
      SaveVariableList( sc, d->m_Args );
      StringBufferAppend( &sc->m_Buffer, '\n' );
    }
    else
      StringBufferAppend( &sc->m_Buffer, "'()\n" );

    StringBufferAppend( &sc->m_Buffer, ")\n" );

    if( sc->m_Buffer.m_Size >= SAVE_BUFFER_FLUSH_LIMIT )
      FlushBuffer( sc );
  }
  StringBufferAppend( &sc->m_Buffer, '\n' );
}

void SaveTrees( SaverContext sc )
{

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
  bool empty_list = v == 0x0;

  if( !empty_list )
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
  }

  if( !empty_list )
    StringBufferAppend( &sc->m_Buffer, ')' );

}

void FlushBuffer( SaverContext sc )
{
  sc->m_Funcs.m_Write( sc, sc->m_Buffer.m_Str, sc->m_Buffer.m_Size );
  StringBufferClear( &sc->m_Buffer );
}

#undef SAVE_BUFFER_FLUSH_LIMIT
