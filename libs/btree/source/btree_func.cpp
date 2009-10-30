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

#include <string.h> // for memset....
#include <stdlib.h>

/*
 * Identifier Functions
 */

void InitIdentifier( Identifier* id )
{
	id->m_Text = 0x0;
	id->m_Hash = INVALID_ID;
	id->m_Line = 0;
}

/*
 * Variable Functions
 */

void InitVariable( Variable* v )
{
	v->m_Type = E_VART_UNDEFINED;
	v->m_Data.m_Integer = 0;
	v->m_Next = 0x0;

	InitIdentifier( &v->m_Id );
}

void DeleteVariableList( Variable* v )
{
	while( v )
	{
		Variable* d = v;
		v = v->m_Next;
		delete d;
	}
}

Variable* FindLastVariable( Variable* v )
{
	while( v && v->m_Next )
		v = v->m_Next;
	return v;
}

void AppendToEndOfList( Variable* s, Variable* v )
{
	while( s && s->m_Next )
		s = s->m_Next;
	if( s )
		s->m_Next = v;
}

int CountElementsInList( Variable* s )
{
	int r = 0;
	while( s )
	{
		++r;
		s = s->m_Next;
	}
	return r;
}

Variable* FindVariableWithIdHash( Variable* s, int hash )
{
	while( s )
	{
		if( s->m_Id.m_Hash == hash )
			return s;
		s = s->m_Next;
	}
	return 0x0;
}

int CountOccourancesOfIdHashInList( Variable* s, int hash )
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

bool VariableIdsAreUniqueInList( Variable* s )
{
	while( s )
	{
		if( CountOccourancesOfIdHashInList( s->m_Next, s->m_Id.m_Hash ) > 0 )
			return false;
		s = s->m_Next;
	}
	return true;
}

bool SafeToConvert( const Variable& v, int to_type )
{
    switch( v.m_Type )
    {
    case E_VART_INTEGER:
        if( to_type == E_VART_INTEGER || to_type == E_VART_FLOAT || to_type == E_VART_BOOL )
            return true;
        break;
    case E_VART_FLOAT:
        if( to_type == E_VART_FLOAT || to_type == E_VART_INTEGER || to_type == E_VART_BOOL )
            return true;
        break;
    case E_VART_STRING:
        if( to_type == E_VART_STRING )
            return true;
        break;
    case E_VART_BOOL:
        if( to_type == E_VART_FLOAT || to_type == E_VART_INTEGER || to_type == E_VART_BOOL )
            return true;
        break;
    }
    return false;
}

int ValueAsInteger( const Variable& v )
{
	int r;
	switch( v.m_Type )
	{
	case E_VART_INTEGER:
		r = v.m_Data.m_Integer;
		break;
	case E_VART_FLOAT:
		r = (int)v.m_Data.m_Float;
		break;
	case E_VART_BOOL:
		r = v.m_Data.m_Bool?1:0;
		break;
	default:
		r = 0;
		break;
	}
    return r;
}

float ValueAsFloat( const Variable& v )
{
	float r;
	switch( v.m_Type )
	{
	case E_VART_INTEGER:
		r = (float)v.m_Data.m_Integer;
		break;
	case E_VART_FLOAT:
		r = v.m_Data.m_Float;
		break;
	case E_VART_BOOL:
		r = v.m_Data.m_Bool?1.0f:0.0f;
		break;
	default:
		r = 0.0f;
		break;
	}
	return r;
}

const char* ValueAsString( const Variable& v )
{
    if( v.m_Type == E_VART_STRING )
        return v.m_Data.m_String;
    return 0x0;
}

bool ValueAsBool( const Variable& v )
{
	bool r;
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
	default:
		r = false;
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

const char* GetVariableListAsString( BehaviorTree* tree, Variable* v )
{
	const char* ret = "";
	int space	= 0;
	int written = 0;
	char* str	= 0x0;
	char* s;
	while( v )
	{
		char tmp[1024];
		int n = sprintf( tmp, "%s = ", v->m_Id.m_Text );


		if( space - (n + written+1) < 0 )
		{
			space	= max( space * 2, n+written+1 );
			str		= (char*)realloc( str, space );
		}

		s = str + written;
		memcpy( s, tmp, n + 1 );
		written += n;

		switch( v->m_Type )
		{
		case E_VART_INTEGER:
			n = sprintf( tmp, "%d", ValueAsInteger( *v ) );
			break;
		case E_VART_FLOAT:
			n = sprintf( tmp, "%f", ValueAsFloat( *v ) );
			break;
		case E_VART_STRING:
			n = sprintf( tmp, "\"%s\"", ValueAsString( *v ) );
			break;
		case E_VART_BOOL:
			n = sprintf( tmp, "%s", ValueAsBool( *v )?"true":"false" );
			break;
		}
		v = v->m_Next;
		if( v )
		{
			tmp[++n] = ',';
			tmp[++n] = ' ';
			tmp[n+1] = 0;
		}

		if( space - (n + written+1) < 0 )
		{
			space	= max( space * 2, n+written+1 );
			str		= (char*)realloc( str, space );
		}

		s = str + written;
		memcpy( s, tmp, n + 1 );
		written += n;
	}

	if( str )
	{
		if( tree )
			ret = tree->RegisterString( str );

		free( str );
		str = 0x0;
	}
	return ret;
}

/*
 * Action functions
 */

void InitAction( Action* a )
{
	InitIdentifier( &a->m_Id );
	a->m_Args		= 0x0;
	a->m_Vars		= 0x0;
	a->m_Declared   = false;
}

/*
 * Decorator functions
 */

void InitDecorator( Decorator* d )
{
	InitIdentifier( &d->m_Id );
	d->m_Args		= 0x0;
	d->m_Vars		= 0x0;
	d->m_Declared   = false;
}

/*
 * Node functions
 */

void InitNode( Node* n )
{
	InitIdentifier( &n->m_Id );
	InitGrist( &n->m_Grist );

	n->m_Tree		= 0x0;
	n->m_Pare		= 0x0;
	n->m_Next		= 0x0;
	n->m_Prev		= 0x0;
	n->m_UserData	= 0x0;

	n->m_Declared	= false;
}

void AppendToEndOfList( Node* s, Node* e )
{
	while( s && s->m_Next )
		s = s->m_Next;
	if( s )
	{
		s->m_Next = e;
		e->m_Prev = s;
	}
}

void SetParentOnChildren( Node* n )
{
	Node* c = GetFirstChild( n );
	while( c )
	{
		c->m_Pare = n;
		c = c->m_Next;
	}
}

Node* GetFirstChild( Node* n )
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
	case E_GRIST_ACTION:
		break;
	}
	return r;
}

void SetFirstChild( Node* n, Node* c )
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
	case E_GRIST_ACTION:
		break;
	}
}

void UnlinkFromSiblings( Node* n )
{
	if( n->m_Prev )
		n->m_Prev->m_Next = n->m_Next;
	if( n->m_Next )
		n->m_Next->m_Prev = n->m_Prev;
	n->m_Prev = 0x0;
	n->m_Next = 0x0;
}

void UnlinkNodeFromParentAndSiblings( Node* n )
{
	if( n->m_Pare )
	{
		Node* fc = GetFirstChild( n->m_Pare );
		if( fc == n )
			SetFirstChild( n->m_Pare, n->m_Next );
	}

	UnlinkFromSiblings( n );
	n->m_Pare = 0x0;
}

int CountChildNodes( Node* n )
{
	Node* c = GetFirstChild( n );
	int retval = 0;
	while( c )
	{
		c = c->m_Next;
		++retval;
	}
	return retval;
}

bool AcceptsMoreChildren( Node* n )
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
	case E_GRIST_DECORATOR:
		return n->m_Grist.m_Decorator.m_Child == 0x0;
		break;
	case E_GRIST_ACTION:
		return false;
		break;
	}
	return false;
}

/*
 * Node Grist functions
 */

void InitGrist( NodeGrist* g )
{
	memset( g, 0, sizeof( NodeGrist ) );
}
