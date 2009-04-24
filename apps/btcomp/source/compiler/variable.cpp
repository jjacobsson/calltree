/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "variable.h"
#include "identifier.h"
#include "../btree/btree.h"

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

int Variable::ValueAsInt() const
{
    if( m_Type == E_VART_INTEGER || m_Type == E_VART_BOOL )
        return m_Data.m_Int;
    if( m_Type == E_VART_FLOAT )
        return (int)m_Data.m_Float;
    return -1;
}

float Variable::ValueAsFloat() const
{
    if( m_Type == E_VART_FLOAT )
        return m_Data.m_Float;
    if( m_Type == E_VART_INTEGER || m_Type == E_VART_BOOL )
        return (float)m_Data.m_Int;
    return -1.0f;
}

const char* Variable::ValueAsString() const
{
    if( m_Type == E_VART_STRING )
        return m_Data.m_String;
    return "";
}

VariableList::VariableList()
    : m_Tree( 0x0 )
    , m_Count( 0 )
{}

VariableList::~VariableList()
{
    if( m_Tree )
    {
        for( int i = 0; i < m_Count; ++i )
            m_Tree->FreeVariable( m_Variables[i] );
    }
}

void VariableList::SetTree( BehaviourTree* bt )
{
    m_Tree = bt;
}

bool VariableList::Append( Variable* v )
{
    if( m_Count >= s_MaxVariableCount )
        return false;

    m_Variables[m_Count] = v;
    ++m_Count;

    return true;
}

int VariableList::Size()
{
    return m_Count;
}

bool VariableSortPred::operator() ( Variable* lhs, Variable* rhs ) const
{
    return lhs->m_Id->m_Hash < rhs->m_Id->m_Hash;
}
