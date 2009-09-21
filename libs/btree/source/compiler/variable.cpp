/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include <btree/variable.h>
#include <btree/identifier.h>
#include <btree/btree.h>

bool Variable::SafeToConvert( int to_type ) const
{
    switch( m_Type )
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

int Variable::ValueAsInteger() const
{
	int r;
	switch( m_Type )
	{
	case E_VART_INTEGER:
		r = m_Data.m_Integer;
		break;
	case E_VART_FLOAT:
		r = (int)m_Data.m_Float;
		break;
	case E_VART_BOOL:
		r = m_Data.m_Bool?1:0;
		break;
	default:
		r = 0;
		break;
	}
    return r;
}

float Variable::ValueAsFloat() const
{
	float r;
	switch( m_Type )
	{
	case E_VART_INTEGER:
		r = (float)m_Data.m_Integer;
		break;
	case E_VART_FLOAT:
		r = m_Data.m_Float;
		break;
	case E_VART_BOOL:
		r = m_Data.m_Bool?1.0f:0.0f;
		break;
	default:
		r = 0.0f;
		break;
	}
	return r;
}

const char* Variable::ValueAsString() const
{
    if( m_Type == E_VART_STRING )
        return m_Data.m_String;
    return 0x0;
}

bool Variable::ValueAsBool() const
{
	bool r;
	switch( m_Type )
	{
	case E_VART_INTEGER:
		r = m_Data.m_Integer != 0;
		break;
	case E_VART_FLOAT:
		r = m_Data.m_Float != 0.0f;
		break;
	case E_VART_BOOL:
		r = m_Data.m_Bool;
		break;
	default:
		r = false;
		break;
	}
	return r;
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


