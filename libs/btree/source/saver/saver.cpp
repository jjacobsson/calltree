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

void save_options( SaverContext );
void save_includes( SaverContext );
void save_actions( SaverContext );
void save_decorators( SaverContext );
void save_trees( SaverContext );
void save_parameter_list( SaverContext, Parameter* );
void save_node_list( SaverContext, Node*, int );
void flush_buffer( SaverContext );

#define SAVE_BUFFER_FLUSH_LIMIT (7 * 1024)

void* get_extra( SaverContext sc )
{
  return sc->m_Extra;
}

void set_extra( SaverContext sc, void* extra )
{
  sc->m_Extra = extra;
}

BehaviorTreeContext get_bt_context( SaverContext sc )
{
  return sc->m_Tree;
}

void save( SaverContext sc, SaverContextFunctions* funcs )
{
  sc->m_Funcs = *funcs;
  append( &sc->m_Buffer, "\n; Options\n\n" );
  save_options( sc );
  append( &sc->m_Buffer, "\n; Includes\n\n" );
  save_includes( sc );
  append( &sc->m_Buffer, "; Actions\n\n" );
  save_actions( sc );
  append( &sc->m_Buffer, "; Decorators\n\n" );
  save_decorators( sc );
  append( &sc->m_Buffer, "; Trees\n\n" );
  save_trees( sc );
  flush_buffer( sc );
}

void save_options( SaverContext sc )
{
  if( sc->m_Tree->m_Options )
  {
    append( &sc->m_Buffer, "(options " );
    save_parameter_list( sc, sc->m_Tree->m_Options );
    append( &sc->m_Buffer, ")\n" );
  }
  if( sc->m_Buffer.m_Size >= SAVE_BUFFER_FLUSH_LIMIT )
    flush_buffer( sc );
}

void save_includes( SaverContext sc )
{
  Include* i = sc->m_Tree->m_Includes;
  while( i )
  {
    const char* translated = sc->m_Funcs.m_Translate( sc, i->m_Name );
    append( &sc->m_Buffer, "(include \"" );
    append( &sc->m_Buffer, translated );
    append( &sc->m_Buffer, "\")\n" );

    if( sc->m_Buffer.m_Size >= SAVE_BUFFER_FLUSH_LIMIT )
      flush_buffer( sc );

    i = i->m_Next;
  }
  append( &sc->m_Buffer, '\n' );
}

void save_actions( SaverContext sc )
{
  int count;
  NamedSymbol* s = access_symbols( sc->m_Tree, &count );
  for( int i = 0; i < count; ++i )
  {
    if( s[i].m_Type != E_ST_ACTION )
      continue;
    if( !s[i].m_Symbol.m_Action->m_Declared )
      continue;
    Action* a = s[i].m_Symbol.m_Action;
    append( &sc->m_Buffer, "(defact " );
    append( &sc->m_Buffer, a->m_Id.m_Text );
    append( &sc->m_Buffer, "\n " );

    save_parameter_list( sc, a->m_Options );
    append( &sc->m_Buffer, "\n " );

    save_parameter_list( sc, a->m_Declarations );
    append( &sc->m_Buffer, '\n' );

    append( &sc->m_Buffer, ")\n" );

    if( sc->m_Buffer.m_Size >= SAVE_BUFFER_FLUSH_LIMIT )
      flush_buffer( sc );
  }
  append( &sc->m_Buffer, '\n' );
}

void save_decorators( SaverContext sc )
{
  int count;
  NamedSymbol* s = access_symbols( sc->m_Tree, &count );
  for( int i = 0; i < count; ++i )
  {
    if( s[i].m_Type != E_ST_DECORATOR )
      continue;
    if( !s[i].m_Symbol.m_Decorator->m_Declared )
      continue;
    Decorator* d = s[i].m_Symbol.m_Decorator;
    append( &sc->m_Buffer, "(defdec " );
    append( &sc->m_Buffer, d->m_Id.m_Text );
    append( &sc->m_Buffer, "\n " );

    save_parameter_list( sc, d->m_Options );
    append( &sc->m_Buffer, "\n " );

    save_parameter_list( sc, d->m_Declarations );
    append( &sc->m_Buffer, '\n' );

    append( &sc->m_Buffer, ")\n" );

    if( sc->m_Buffer.m_Size >= SAVE_BUFFER_FLUSH_LIMIT )
      flush_buffer( sc );
  }
  append( &sc->m_Buffer, '\n' );
}

void save_trees( SaverContext sc )
{
  int count;
  NamedSymbol* s = access_symbols( sc->m_Tree, &count );
  for( int i = 0; i < count; ++i )
  {
    if( s[i].m_Type != E_ST_TREE )
      continue;
    if( !s[i].m_Symbol.m_Tree->m_Declared )
      continue;
    BehaviorTree* t = s[i].m_Symbol.m_Tree;
    append( &sc->m_Buffer, "(deftree " );
    append( &sc->m_Buffer, t->m_Id.m_Text );
    append( &sc->m_Buffer, ' ' );
    save_node_list( sc, t->m_Root, 1 );
    append( &sc->m_Buffer, ")\n\n" );


    if( sc->m_Buffer.m_Size >= SAVE_BUFFER_FLUSH_LIMIT )
      flush_buffer( sc );
  }
}

void save_parameter_declaration( SaverContext sc, Parameter* v )
{
  switch( v->m_Type )
  {
  case E_VART_INTEGER:
    append( &sc->m_Buffer, "int32 " );
    append( &sc->m_Buffer, v->m_Id.m_Text );
    break;
  case E_VART_FLOAT:
    append( &sc->m_Buffer, "float " );
    append( &sc->m_Buffer, v->m_Id.m_Text );
    break;
  case E_VART_STRING:
    append( &sc->m_Buffer, "string " );
    append( &sc->m_Buffer, v->m_Id.m_Text );
    break;
  case E_VART_HASH:
    append( &sc->m_Buffer, "hash " );
    append( &sc->m_Buffer, v->m_Id.m_Text );
    break;
  case E_VART_BOOL:
    append( &sc->m_Buffer, "bool " );
    append( &sc->m_Buffer, v->m_Id.m_Text );
    break;
  case E_VART_UNDEFINED:
  case E_MAX_VARIABLE_TYPE:
    break;
  }
}

void save_variable_assignment( SaverContext sc, Parameter* v )
{
  char tmp[128];
  switch( v->m_Type )
  {
  case E_VART_INTEGER:
    append( &sc->m_Buffer, v->m_Id.m_Text );
    append( &sc->m_Buffer, ' ' );
    sprintf( tmp, "%d", as_integer( *v ) );
    append( &sc->m_Buffer, tmp );
    break;
  case E_VART_FLOAT:
    append( &sc->m_Buffer, v->m_Id.m_Text );
    append( &sc->m_Buffer, ' ' );
    sprintf( tmp, "%f", as_float( *v ) );
    append( &sc->m_Buffer, tmp );
    break;
  case E_VART_STRING:
    append( &sc->m_Buffer, v->m_Id.m_Text );
    append( &sc->m_Buffer, " \"" );
    append( &sc->m_Buffer, as_string( *v )->m_Raw );
    append( &sc->m_Buffer, '\"' );
    break;
  case E_VART_HASH:
    append( &sc->m_Buffer, v->m_Id.m_Text );
    append( &sc->m_Buffer, ' ' );
    sprintf( tmp, "0x%08x", as_hash( *v ) );
    append( &sc->m_Buffer, tmp );
  case E_VART_BOOL:
    append( &sc->m_Buffer, v->m_Id.m_Text );
    append( &sc->m_Buffer, ' ' );
    append( &sc->m_Buffer, as_bool( *v ) ? "true" : "false" );
    break;
  case E_VART_UNDEFINED:
  case E_MAX_VARIABLE_TYPE:
    break;
  }
}

void save_parameter_list( SaverContext sc, Parameter* v )
{
  if( !v )
  {
    append( &sc->m_Buffer, "null" );
    return;
  }

  append( &sc->m_Buffer, "(" );
  while( v )
  {
    append( &sc->m_Buffer, '(' );
    if( v->m_ValueSet )
      save_variable_assignment( sc, v );
    else
      save_parameter_declaration( sc, v );
    append( &sc->m_Buffer, ')' );
    v = v->m_Next;
    if( v )
      append( &sc->m_Buffer, ' ' );

    if( sc->m_Buffer.m_Size >= SAVE_BUFFER_FLUSH_LIMIT )
      flush_buffer( sc );
  }
  append( &sc->m_Buffer, ')' );
}

void save_node( SaverContext, Node*, int );

void append_depth( SaverContext sc, int depth )
{
  //Negative scope... hmm...
  if( depth <= 0 )
    return;

  //This is to ensure that we don't get a buffer-overrun in the "space_buff" string.
  if( depth >= 500 )
    depth = 500;

  char depth_buff[ 128 ];
  char space_buff[ 1024 ];
  sprintf( depth_buff, "%%-%ds", depth );
  sprintf( space_buff, depth_buff, "" );
  append( &sc->m_Buffer, space_buff );
}

void save_sequence( SaverContext sc, Node* n, int depth )
{
  append( &sc->m_Buffer, "(sequence " );
  save_node_list( sc, get_first_child( n ), depth );
  append( &sc->m_Buffer, ")\n" );
}

void save_selector( SaverContext sc, Node* n, int depth )
{
  append( &sc->m_Buffer, "(selector " );
  save_node_list( sc, get_first_child( n ), depth );
  append( &sc->m_Buffer, ")\n" );
}

void save_parallel( SaverContext sc, Node* n, int depth )
{
  append( &sc->m_Buffer, "(parallel " );
  save_node_list( sc, get_first_child( n ), depth );
  append( &sc->m_Buffer, ")\n" );
}

void save_dyn_selector( SaverContext sc, Node* n, int depth )
{
  append( &sc->m_Buffer, "(dyn_selector " );
  save_node_list( sc, get_first_child( n ), depth );
  append( &sc->m_Buffer, ")\n" );
}

void save_succeed( SaverContext sc, Node*, int )
{
  append( &sc->m_Buffer, "(succeed)\n" );
}
void save_fail( SaverContext sc, Node*, int )
{
  append( &sc->m_Buffer, "(fail)\n" );
}
void save_work( SaverContext sc, Node*, int )
{
  append( &sc->m_Buffer, "(work)\n" );
}

void save_decorator( SaverContext sc, Node* n, int depth )
{
  append( &sc->m_Buffer, "(decorator '" );
  append( &sc->m_Buffer, n->m_Grist.m_Decorator.m_Decorator->m_Id.m_Text );
  append( &sc->m_Buffer, ' ' );
  save_parameter_list( sc, n->m_Grist.m_Decorator.m_Parameters );

  if( n->m_Grist.m_Decorator.m_Child )
  {
    append( &sc->m_Buffer, '\n' );
    save_node( sc, n->m_Grist.m_Decorator.m_Child, depth + 1 );
    append_depth( sc, depth );
  }
  else
  {
    append( &sc->m_Buffer, " null" );
  }

  append( &sc->m_Buffer, ")\n" );
}

void save_action( SaverContext sc, Node* n, int )
{
  append( &sc->m_Buffer, "(action '" );
  append( &sc->m_Buffer, n->m_Grist.m_Action.m_Action->m_Id.m_Text );
  append( &sc->m_Buffer, ' ' );
  save_parameter_list( sc, n->m_Grist.m_Action.m_Parameters );
  append( &sc->m_Buffer, ")\n" );
}

void save_tree( SaverContext sc, Node* n, int )
{
  append( &sc->m_Buffer, "(tree '" );
  append( &sc->m_Buffer, n->m_Grist.m_Tree.m_Tree->m_Id.m_Text );
  append( &sc->m_Buffer, ")\n" );
}

void save_node( SaverContext sc, Node* n, int depth )
{
  append_depth( sc, depth );
  depth++;
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_SEQUENCE:
    save_sequence( sc, n, depth );
    break;
  case E_GRIST_SELECTOR:
    save_selector( sc, n, depth );
    break;
  case E_GRIST_PARALLEL:
    save_parallel( sc, n, depth );
    break;
  case E_GRIST_DYN_SELECTOR:
    save_dyn_selector( sc, n, depth );
    break;
  case E_GRIST_SUCCEED:
    save_succeed( sc, n, depth );
    break;
  case E_GRIST_FAIL:
    save_fail( sc, n, depth );
    break;
  case E_GRIST_WORK:
    save_work( sc, n, depth );
    break;
  case E_GRIST_DECORATOR:
    save_decorator( sc, n, depth );
    break;
  case E_GRIST_ACTION:
    save_action( sc, n, depth );
    break;
  case E_GRIST_TREE:
    save_tree( sc, n, depth );
    break;
  case E_GRIST_UNKOWN:
  case E_MAX_GRIST_TYPES:
    /* Warning killer */
    break;
  }
  if( sc->m_Buffer.m_Size >= SAVE_BUFFER_FLUSH_LIMIT )
    flush_buffer( sc );
}

void save_node_list( SaverContext sc, Node* n, int depth )
{
  append( &sc->m_Buffer, "(" );

  if( !n )
  {
    append( &sc->m_Buffer, "null)" );
    return;
  }

  append( &sc->m_Buffer, '\n' );

  Node* it = n;
  while( it )
  {
    save_node( sc, it, depth + 1 );
    it = it->m_Next;
  }

  append_depth( sc, depth );
  append( &sc->m_Buffer, ")\n" );
  if( depth > 1 )
    append_depth( sc, depth - 1 );
}

void flush_buffer( SaverContext sc )
{
  sc->m_Funcs.m_Write( sc, sc->m_Buffer.m_Str, sc->m_Buffer.m_Size );
  clear( &sc->m_Buffer );
}

#undef SAVE_BUFFER_FLUSH_LIMIT
