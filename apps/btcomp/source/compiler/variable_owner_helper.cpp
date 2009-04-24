/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "variable_owner_helper.h"
#include "../code/bt_program.h"
#include "../btree/btree.h"
#include "node.h"

#include <algorithm>

using namespace callback;

VariableOwner::VariableOwner()
    : m_Variables( 0x0 )
    , m_bssStart( 0 )
{
}

VariableOwner::~VariableOwner()
{
    if( m_Chaff && m_Chaff->m_Tree )
    {
        m_Chaff->m_Tree->FreeVariableList( m_Variables );
    }
}


void VariableOwner::StoreVariablesInDataSection( BehaviourTree* bt, VariableList* declaration )
{
    if( !m_Variables || !declaration || declaration->Size() == 0 )
        return;

    m_Data.clear();
    m_bssStart = bt->m_B.Push( sizeof(void*) * declaration->Size(), sizeof(void*) );

    VariableSortPred pred;
    VariableList::iterator it, it_e( declaration->end() ), it_f;
    for( it = declaration->begin(); it != it_e; ++it )
    {
        it_f = std::lower_bound( m_Variables->begin(), m_Variables->end(), *it, pred );
        Variable* v = *it_f;

        switch( (*it)->m_Type )
        {
        case Variable::E_VART_INTEGER:
            m_Data.push_back( bt->m_D.PushInteger( v->ValueAsInt() ) );
            break;
        case Variable::E_VART_FLOAT:
            m_Data.push_back( bt->m_D.PushFloat( v->ValueAsFloat() ) );
            break;
        case Variable::E_VART_STRING:
            m_Data.push_back( bt->m_D.PushString( v->ValueAsString() ) );
            break;
        case Variable::E_VART_BOOL:
            m_Data.push_back( bt->m_D.PushInteger( v->ValueAsInt() ) );
            break;
        default:
            //assert( false );
            break;
        }
    }
}

void VariableOwner::GenerateVariableInstructions( BehaviourTree* bt )
{
    std::vector<int>::iterator it, it_e( m_Data.end() );
    int i = 0;
    for( it = m_Data.begin(); it != it_e; ++it, ++i )
    {
        //Store a pointer to a variable in the data section in the bss section.
        bt->m_I.Push( INST_STORE_PD_IN_B, m_bssStart + (sizeof(void*) * i), *it, 0 );
    }

}

void VariableOwner::SetupVariableRegistry( BehaviourTree* bt )
{
    if( !m_Data.empty() )
    {
        // Load the user data registry with a pointer to the bss structure for variables
        bt->m_I.Push( INST_STORE_PB_IN_R, 2, m_bssStart, 0 );
    }
    else
    {
        //Load the user data registry with a null pointer
        bt->m_I.Push( INST_LOAD_REGISTRY, 2, 0, 0 );
    }
}

void VariableOwner::SetVariableList( VariableList* variables )
{
    m_Variables = variables;
}

VariableList* VariableOwner::GetVariableList()
{
    return m_Variables;
}

