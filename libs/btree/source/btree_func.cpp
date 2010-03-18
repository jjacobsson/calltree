/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include <btree/btree_data.h>
#include <btree/btree_func.h>
#include <btree/btree.h>

#include <other/lookup3.h>

#include <string.h> // for memset....
#include <stdlib.h>
#include <stdio.h>

const hash_t g_keyword_hash_values[] = {
  hashlittle( "deftree" ),
  hashlittle( "defact" ),
  hashlittle( "defdec" ),
  hashlittle( "tree" ),
  hashlittle( "sequence" ),
  hashlittle( "selector" ),
  hashlittle( "parallel" ),
  hashlittle( "dyn_selector" ),
  hashlittle( "succeed" ),
  hashlittle( "fail" ),
  hashlittle( "work" ),
  hashlittle( "action" ),
  hashlittle( "decorator" ),
  hashlittle( "int32" ),
  hashlittle( "bool" ),
  hashlittle( "float" ),
  hashlittle( "string" ),
  hashlittle( "hash" ),
  hashlittle( "include" ),
  hashlittle( "true" ),
  hashlittle( "false" )
};

bool is_btree_keyword( const char* str )
{
  hash_t t = hashlittle( str );
  int s = sizeof( g_keyword_hash_values ) / sizeof( hash_t );
  for( int i = 0; i < s; ++i )
  {
    if( g_keyword_hash_values[i] == t )
      return true;
  }
  return false;
}

/*
 * Locator functions
 */

void init( Locator* l )
{
  l->m_Buffer = 0x0;
  l->m_LineNo = 0;
}

/*
 * Identifier Functions
 */

void init( Identifier* id )
{
  id->m_Text = 0x0;
  id->m_Hash = INVALID_ID;
}

/*
 * Variable Functions
 */

void init( Parameter* v )
{
  v->m_Type = E_VART_UNDEFINED;
  v->m_Data.m_String.m_Parsed = 0x0;
  v->m_Data.m_String.m_Raw = 0x0;
  v->m_Next = 0x0;
  v->m_ValueSet = false;
  init( &v->m_Id );
  init( &v->m_Locator );
}

Parameter* find_last( Parameter* v )
{
  while( v && v->m_Next )
    v = v->m_Next;
  return v;
}

void append_to_end( Parameter* s, Parameter* v )
{
  while( s && s->m_Next )
    s = s->m_Next;
  if( s )
    s->m_Next = v;
}

int count_elements( Parameter* s )
{
  int r = 0;
  while( s )
  {
    ++r;
    s = s->m_Next;
  }
  return r;
}

Parameter* find_by_hash( Parameter* s, hash_t hash )
{
  while( s )
  {
    if( s->m_Id.m_Hash == hash )
      return s;
    s = s->m_Next;
  }
  return 0x0;
}

int count_occourances_of_hash_in_list( Parameter* s, hash_t hash )
{
  int r = 0;
  while( s )
  {
    if( s->m_Id.m_Hash == hash )
      ++r;
    s = s->m_Next;
  }
  return r;
}

bool id_hashes_are_unique_in_list( Parameter* s )
{
  while( s )
  {
    if( count_occourances_of_hash_in_list( s->m_Next, s->m_Id.m_Hash ) > 0 )
      return false;
    s = s->m_Next;
  }
  return true;
}

bool safe_to_convert( const Parameter& v, int to_type )
{
  switch( v.m_Type )
  {
  case E_VART_INTEGER:
    if( to_type == E_VART_INTEGER || to_type == E_VART_FLOAT || to_type
        == E_VART_BOOL || to_type == E_VART_HASH )
      return true;
    break;
  case E_VART_FLOAT:
    if( to_type == E_VART_FLOAT || to_type == E_VART_INTEGER || to_type
        == E_VART_BOOL )
      return true;
    break;
  case E_VART_STRING:
    if( to_type == E_VART_STRING || to_type == E_VART_HASH )
      return true;
    break;
  case E_VART_BOOL:
    if( to_type == E_VART_FLOAT || to_type == E_VART_INTEGER || to_type
        == E_VART_BOOL )
      return true;
    break;
  case E_VART_HASH:
    if( to_type == E_VART_HASH || to_type == E_VART_INTEGER )
      return true;
    break;
  case E_VART_UNDEFINED:
  case E_MAX_VARIABLE_TYPE:
    break;
  }
  return false;
}

int as_integer( const Parameter& v )
{
  int r = 0;
  switch( v.m_Type )
  {
  case E_VART_INTEGER:
    r = v.m_Data.m_Integer;
    break;
  case E_VART_FLOAT:
    r = (int)v.m_Data.m_Float;
    break;
  case E_VART_BOOL:
    r = v.m_Data.m_Bool ? 1 : 0;
    break;
  case E_VART_HASH:
    r = (int)v.m_Data.m_Hash;
    break;
  case E_VART_UNDEFINED:
  case E_VART_STRING:
  case E_MAX_VARIABLE_TYPE:
    r = 0;
    break;
  }
  return r;
}

float as_float( const Parameter& v )
{
  float r = 0.0f;
  switch( v.m_Type )
  {
  case E_VART_INTEGER:
    r = (float)v.m_Data.m_Integer;
    break;
  case E_VART_FLOAT:
    r = v.m_Data.m_Float;
    break;
  case E_VART_BOOL:
    r = v.m_Data.m_Bool ? 1.0f : 0.0f;
    break;
  case E_VART_UNDEFINED:
  case E_VART_STRING:
  case E_VART_HASH:
  case E_MAX_VARIABLE_TYPE:
    r = 0.0f;
    break;
  }
  return r;
}

const StringData* as_string( const Parameter& v )
{
  if( v.m_Type == E_VART_STRING )
    return &v.m_Data.m_String;
  return 0x0;
}

bool as_bool( const Parameter& v )
{
  bool r = false;
  switch( v.m_Type )
  {
  case E_VART_INTEGER:
    r = v.m_Data.m_Integer != 0;
    break;
  case E_VART_FLOAT:
    r = v.m_Data.m_Float != 0.0f;
    break;
  case E_VART_BOOL:
    r = v.m_Data.m_Bool;
    break;
  case E_VART_UNDEFINED:
  case E_VART_STRING:
  case E_VART_HASH:
  case E_MAX_VARIABLE_TYPE:
    r = false;
    break;
  }
  return r;
}

hash_t as_hash( const Parameter& v )
{
  hash_t r = 0;
  switch( v.m_Type )
  {
  case E_VART_INTEGER:
    r = (hash_t)v.m_Data.m_Integer;
    break;
  case E_VART_FLOAT:
  case E_VART_BOOL:
    break;
  case E_VART_STRING:
    r = hashlittle( v.m_Data.m_String.m_Parsed );
    break;
  case E_VART_HASH:
    r = v.m_Data.m_Hash;
    break;
  case E_VART_UNDEFINED:
  case E_MAX_VARIABLE_TYPE:
    break;
  }
  return r;
}

int max( int a, int b )
{
  if( a > b )
    return a;
  return b;
}

const char* list_as_string( BehaviorTreeContext tree, Parameter* v )
{
  const char* ret = "";
  int space = 0;
  int written = 0;
  char* str = 0x0;
  char* s;
  while( v )
  {
    char tmp[1024];
    int n = sprintf( tmp, "%s = ", v->m_Id.m_Text );

    if( space - (n + written + 1) < 0 )
    {
      space = max( space * 2, n + written + 1 );
      str = (char*)realloc( str, space );
    }

    s = str + written;
    memcpy( s, tmp, n + 1 );
    written += n;

    switch( v->m_Type )
    {
    case E_VART_INTEGER:
      n = sprintf( tmp, "%d", as_integer( *v ) );
      break;
    case E_VART_FLOAT:
      n = sprintf( tmp, "%f", as_float( *v ) );
      break;
    case E_VART_STRING:
      n = sprintf( tmp, "\"%s\"", as_string( *v )->m_Parsed );
      break;
    case E_VART_BOOL:
      n = sprintf( tmp, "%s", as_bool( *v ) ? "true" : "false" );
      break;
    case E_VART_HASH:
      n = sprintf( tmp, "0x%08x", as_hash( *v ) );
      break;
    case E_VART_UNDEFINED:
    case E_MAX_VARIABLE_TYPE:
      break;
    }
    v = v->m_Next;
    if( v )
    {
      tmp[++n] = ',';
      tmp[++n] = ' ';
      tmp[n + 1] = 0;
    }

    if( space - (n + written + 1) < 0 )
    {
      space = max( space * 2, n + written + 1 );
      str = (char*)realloc( str, space );
    }

    s = str + written;
    memcpy( s, tmp, n + 1 );
    written += n;
  }

  if( str )
  {
    if( tree )
      ret = register_string( tree, str );

    free( str );
    str = 0x0;
  }
  return ret;
}

/*
 * BehaviorTree Functions
 */

void init( BehaviorTree* t )
{
  init( &t->m_Id );
  init( &t->m_Locator );
  t->m_Root = 0x0;
  t->m_UserData = 0x0;
  t->m_Declared = false;
}

void set_parent_on_children( BehaviorTree* t )
{
  Node* c = t->m_Root;
  while( c )
  {
    c->m_Pare.m_Type = E_NP_TREE;
    c->m_Pare.m_Tree = t;
    c = c->m_Next;
  }
}

void unlink_from_children( BehaviorTree* t )
{
  Node* c = t->m_Root;
  while( c )
  {
    c->m_Pare.m_Tree = 0x0;
    c->m_Pare.m_Type = E_NP_UNKOWN;
    c = c->m_Next;
  }
}

/*
 * Action functions
 */

void init( Action* a )
{
  init( &a->m_Id );
  init( &a->m_Locator );
  a->m_Declarations = 0x0;
  a->m_Options = 0x0;
  a->m_Declared = false;
}

/*
 * Decorator functions
 */

void init( Decorator* d )
{
  init( &d->m_Id );
  init( &d->m_Locator );
  d->m_Declarations = 0x0;
  d->m_Options = 0x0;
  d->m_Declared = false;
}

/*
 * Node functions
 */

void init( Node* n )
{
  init( &n->m_Grist );
  init( &n->m_Locator );
  n->m_Pare.m_Type = E_NP_UNKOWN;
  n->m_Pare.m_Node = 0x0;
  n->m_NodeId = 0;
  n->m_Next = 0x0;
  n->m_Prev = 0x0;
  n->m_UserData = 0x0;
}

void free_node( BehaviorTreeContext ctx, Node* n )
{
  Parameter* v = 0x0;
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_DECORATOR:
    v = n->m_Grist.m_Decorator.m_Parameters;
    break;
  case E_GRIST_ACTION:
    v = n->m_Grist.m_Action.m_Parameters;
    break;
  case E_GRIST_UNKOWN:
  case E_GRIST_SEQUENCE:
  case E_GRIST_SELECTOR:
  case E_GRIST_PARALLEL:
  case E_GRIST_DYN_SELECTOR:
  case E_GRIST_SUCCEED:
  case E_GRIST_FAIL:
  case E_GRIST_WORK:
  case E_GRIST_TREE:
  case E_MAX_GRIST_TYPES:
    /* Warning killers */
    v = 0x0;
    break;
  }
  while( v )
  {
    Parameter* n = v->m_Next;
    free_object( ctx, v );
    v = n;
  }
  free_object( ctx, n );
}

void append_to_end( Node* s, Node* e )
{
  while( s && s->m_Next )
    s = s->m_Next;
  if( s )
  {
    s->m_Next = e;
    e->m_Prev = s;
  }
}

void set_parent_on_children( Node* n )
{
  Node* c = get_first_child( n );
  while( c )
  {
    c->m_Pare.m_Type = E_NP_NODE;
    c->m_Pare.m_Node = n;
    c = c->m_Next;
  }
}

Node* get_first_child( Node* n )
{
  if( !n )
    return 0x0;

  Node* r = 0x0;
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_SEQUENCE:
    r = n->m_Grist.m_Sequence.m_FirstChild;
    break;
  case E_GRIST_SELECTOR:
    r = n->m_Grist.m_Selector.m_FirstChild;
    break;
  case E_GRIST_PARALLEL:
    r = n->m_Grist.m_Parallel.m_FirstChild;
    break;
  case E_GRIST_DYN_SELECTOR:
    r = n->m_Grist.m_DynSelector.m_FirstChild;
    break;
  case E_GRIST_DECORATOR:
    r = n->m_Grist.m_Decorator.m_Child;
    break;
  case E_GRIST_UNKOWN:
  case E_GRIST_ACTION:
  case E_GRIST_TREE:
  case E_GRIST_FAIL:
  case E_GRIST_SUCCEED:
  case E_GRIST_WORK:
  case E_MAX_GRIST_TYPES:
    break;
  }
  return r;
}

Node* get_first_child( const NodeParent& p )
{
  switch( p.m_Type )
  {
  case E_NP_NODE:
    return get_first_child( p.m_Node );
    break;
  case E_NP_TREE:
    return p.m_Tree->m_Root;
    break;
  case E_NP_UNKOWN:
  case E_MAX_NODE_PARENT_TYPES:
    break;
  }
  return 0x0;
}

void set_first_child( Node* n, Node* c )
{
  if( !n )
    return;

  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_SEQUENCE:
    n->m_Grist.m_Sequence.m_FirstChild = c;
    break;
  case E_GRIST_SELECTOR:
    n->m_Grist.m_Selector.m_FirstChild = c;
    break;
  case E_GRIST_PARALLEL:
    n->m_Grist.m_Parallel.m_FirstChild = c;
    break;
  case E_GRIST_DYN_SELECTOR:
    n->m_Grist.m_DynSelector.m_FirstChild = c;
    break;
  case E_GRIST_DECORATOR:
    n->m_Grist.m_Decorator.m_Child = c;
    break;
  case E_GRIST_UNKOWN:
  case E_GRIST_TREE:
  case E_GRIST_SUCCEED:
  case E_GRIST_FAIL:
  case E_GRIST_WORK:
  case E_GRIST_ACTION:
  case E_MAX_GRIST_TYPES:
    break;
  }
}

void set_first_child( const NodeParent& p, Node* c )
{
  switch( p.m_Type )
  {
  case E_NP_NODE:
    set_first_child( p.m_Node, c );
    break;
  case E_NP_TREE:
    p.m_Tree->m_Root = c;
    break;
  case E_NP_UNKOWN:
  case E_MAX_NODE_PARENT_TYPES:
    break;
  }
}

void unlink_from_siblings( Node* n )
{
  if( n->m_Prev )
    n->m_Prev->m_Next = n->m_Next;
  if( n->m_Next )
    n->m_Next->m_Prev = n->m_Prev;
  n->m_Prev = 0x0;
  n->m_Next = 0x0;
}

void unlink_from_parent_and_siblings( Node* n )
{
  switch( n->m_Pare.m_Type )
  {
  case E_NP_NODE:
    if( get_first_child( n->m_Pare.m_Node ) == n )
      set_first_child( n->m_Pare.m_Node, n->m_Next );
    break;
  case E_NP_TREE:
    if( n->m_Pare.m_Tree->m_Root == n )
      n->m_Pare.m_Tree->m_Root = n->m_Next;
    break;
  case E_NP_UNKOWN:
  case E_MAX_NODE_PARENT_TYPES:
    break;
  }
  unlink_from_siblings( n );
  n->m_Pare.m_Type = E_NP_UNKOWN;
  n->m_Pare.m_Node = 0x0;
}

int count_children( Node* n )
{
  Node* c = get_first_child( n );
  int retval = 0;
  while( c )
  {
    c = c->m_Next;
    ++retval;
  }
  return retval;
}

bool accepts_more_children( Node* n )
{
  if( !n )
    return false;
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_SEQUENCE:
  case E_GRIST_SELECTOR:
  case E_GRIST_PARALLEL:
  case E_GRIST_DYN_SELECTOR:
    return true;
    break;
  case E_GRIST_SUCCEED:
  case E_GRIST_FAIL:
  case E_GRIST_WORK:
    return false;
    break;
  case E_GRIST_DECORATOR:
    return n->m_Grist.m_Decorator.m_Child == 0x0;
    break;
  case E_GRIST_ACTION:
  case E_GRIST_TREE:
    return false;
    break;
  case E_GRIST_UNKOWN:
  case E_MAX_GRIST_TYPES:
    return false;
    break;
  }
  return false;
}

Parameter* get_parameters( Node* n )
{
  if( !n )
    return 0x0;
  Parameter* r = 0x0;
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_SEQUENCE:
  case E_GRIST_SELECTOR:
  case E_GRIST_PARALLEL:
  case E_GRIST_DYN_SELECTOR:
  case E_GRIST_SUCCEED:
  case E_GRIST_FAIL:
  case E_GRIST_WORK:
    break;
  case E_GRIST_DECORATOR:
    r = n->m_Grist.m_Decorator.m_Parameters;
    break;
  case E_GRIST_ACTION:
    r = n->m_Grist.m_Action.m_Parameters;
    break;
  case E_GRIST_TREE:
  case E_GRIST_UNKOWN:
  case E_MAX_GRIST_TYPES:
    break;
  }
  return r;
}

BehaviorTree* find_parent_tree( const NodeParent& p )
{
  switch( p.m_Type )
  {
  case E_NP_TREE:
    return p.m_Tree;
    break;
  case E_NP_NODE:
    return find_parent_tree( p.m_Node->m_Pare );
    break;
  case E_NP_UNKOWN:
  case E_MAX_NODE_PARENT_TYPES:
    break;
  }
  return 0x0;
}

bool contains_reference_to_tree( Node* n, BehaviorTree* tree )
{
  if( !n )
    return false;

  Node* it;

  if( n->m_Grist.m_Type == E_GRIST_TREE )
  {
    if( n->m_Grist.m_Tree.m_Tree == tree )
      return true;

    if( !n->m_Grist.m_Tree.m_Tree )
      return false;

    it = n->m_Grist.m_Tree.m_Tree->m_Root;
    while( it )
    {
      if( contains_reference_to_tree( it, tree ) )
        return true;
      it = it->m_Next;
    }
  }

  it = get_first_child( n );
  while( it )
  {
    if( contains_reference_to_tree( it, tree ) )
      return true;
    it = it->m_Next;
  }
  return false;
}


/*
 * Node Grist functions
 */

void init( NodeGrist* g )
{
  memset( g, 0, sizeof(NodeGrist) );
}

/*
 * String Buffer Functions
 */


void init( Allocator& a, StringBuffer* sb, int initial_size )
{
  sb->m_Allocator   = a;
  sb->m_Size        = 0;
  sb->m_Capacity    = 0;
  sb->m_Str         = 0x0;
  grow( sb, initial_size );
  sb->m_Str[0]      = 0;
}

void destroy( StringBuffer* sb )
{
  sb->m_Allocator.m_Free( sb->m_Str );
  sb->m_Str         = 0x0;
  sb->m_Size        = 0;
  sb->m_Capacity    = 0;
  memset( sb, 0xdeadbeef, sizeof(StringBuffer) );
}

void append( StringBuffer* sb, char c )
{
  if( sb->m_Size >= sb->m_Capacity - 1 )
    grow( sb, 128 );
  sb->m_Str[sb->m_Size++]   = c;
  sb->m_Str[sb->m_Size]     = 0;
}

void append( StringBuffer* sb, const char* str )
{
  if( !str ) return;

  int l = strlen( str );
  append( sb, str, l );
}

void append( StringBuffer* sb, const char * str, int l )
{
  if( l == 0 ) return;

  if( sb->m_Size >= sb->m_Capacity - (l + 1) )
    grow( sb, l + 1 );
  memcpy( sb->m_Str + sb->m_Size, str, l );
  sb->m_Size += l;
  sb->m_Str[sb->m_Size] = 0;
}

void clear( StringBuffer* sb )
{
  if( sb->m_Capacity <= 0 )
    return;
  sb->m_Size    = 0;
  sb->m_Str[0]  = 0;
}

void grow( StringBuffer* sb, int min )
{
  int ns = sb->m_Capacity + (128>min?128:min);
  char* t = (char*)sb->m_Allocator.m_Alloc( ns );
  if( sb->m_Str )
  {
    if( sb->m_Size > 0 )
      memcpy( t, sb->m_Str, sb->m_Size + 1 );

    sb->m_Allocator.m_Free( sb->m_Str );

    sb->m_Str       = t;
    sb->m_Capacity  = ns;
  }
  else
  {
    sb->m_Capacity  = ns;
    sb->m_Str       = t;
  }
}

