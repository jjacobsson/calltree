/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#include <btree/function_grist.h>
#include <btree/identifier.h>
#include <btree/btree.h>

#include <algorithm>

FunctionGrist::FunctionGrist()
    : m_bssSize( 0 )
    , m_Variables( 0x0 )
    , m_Tree( 0x0 )
{
}

FunctionGrist::~FunctionGrist()
{
	DeleteVariableList( m_Variables );
}

bool FunctionGrist::ValiadateVariables( Variable* variables ) const
{
    if( (!m_Variables && variables) || (m_Variables && !variables) )
        return false;

/*
    VariableSortPred pred;
    std::sort( variables->begin(), variables->end(), pred );

    int s = m_Variables->Size();

    if( variables->Size() != s )
        return false;

    Variable *v1, *v2;

    for( int i = 0; i < s; ++i )
    {
        v1 = (*m_Variables)[i];

        VariableList::iterator it = std::lower_bound( variables->begin(), variables->end(), v1, pred );

        if( it == variables->end() )
            return false;

        v2 = *it;

        if( v1->m_Id->m_Hash != v2->m_Id->m_Hash )
            return false;

        if( v1->SafeToConvert( v2->m_Type ) )
            continue;

        return false;
    }
*/
    return true;
}
