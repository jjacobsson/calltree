/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "parallel_node_grist.h"
#include "../node.h"
#include "../node_list.h"
#include "../../code/bt_program.h"
#include "../../btree/btree.h"

#include <vector>

using namespace callback;

ParallelNodeGrist::ParallelNodeGrist( )
    : m_Children( 0x0 )
{
}

ParallelNodeGrist::~ParallelNodeGrist()
{
    if( m_Chaff && m_Chaff->m_Tree )
    {
        m_Chaff->m_Tree->FreeNodeList( m_Children );
        m_Children = 0x0;
    }
}

void ParallelNodeGrist::GenerateConstructionCode( BehaviourTree* bt )
{
    // Enter Debug scope
    bt->m_I.PushDebugScope( bt, m_Chaff, ACT_CONSTRUCT );

    //Alloc storage space for the success counter
    m_bss_SuccessCounter = bt->m_B.Push( sizeof( int ), 4 );

    int i = 0;
    NodeList::const_iterator it( m_Children->begin() ), it_e( m_Children->end() );
    for( ; it != it_e; ++it, ++i )
    {
        //call child-node construction code
        (*it)->m_Grist->GenerateConstructionCode( bt );
    }

    // Exit Debug scope
    bt->m_I.PopDebugScope( bt, m_Chaff, ACT_CONSTRUCT );
}

void ParallelNodeGrist::GenerateExecutionCode( BehaviourTree* bt )
{
    // Enter Debug scope
    bt->m_I.PushDebugScope( bt, m_Chaff, ACT_EXECUTE );

    //Set the success counter to 0
    bt->m_I.Push( INST__STORE_C_IN_B, m_bss_SuccessCounter, 0, 0 );

    typedef std::vector<int> PatchList;
    PatchList exit_fail;

    int i = 0;
    NodeList::const_iterator it( m_Children->begin() ), it_e( m_Children->end() );
    for( ; it != it_e; ++it, ++i )
    {
        //call child-node execution code
        (*it)->m_Grist->GenerateExecutionCode( bt );

        //jump out if failed
        exit_fail.push_back( bt->m_I.Count() );
        bt->m_I.Push( INST_JABC_R_EQUA_C, 0xffffffff, E_NODE_FAIL, 0 );

        //Skip count up if running
        bt->m_I.Push( INST_JABC_R_DIFF_C, bt->m_I.Count() + 2, E_NODE_SUCCESS, 0 );

        //Count up number of successful child nodes.
        bt->m_I.Push( INST__INC_BSSVALUE, m_bss_SuccessCounter, 1, 0 );
    }

    //Set return value to running
    bt->m_I.Push( INST__STORE_C_IN_R, E_NODE_RUNNING, 0, 0 );
    //Jump out if number any child was non-successfull.
    bt->m_I.Push( INST_JABC_C_DIFF_B, bt->m_I.Count() + 2, m_Children->Size(), m_bss_SuccessCounter );
    //Set return value to success (skiped if m_bss_SuccessCounter != m_Children->Size())
    bt->m_I.Push( INST__STORE_C_IN_R, E_NODE_SUCCESS, 0, 0 );

    //Patch exit-fail jumps
    int exit_point = bt->m_I.Count();
    int s = exit_fail.size();
    for( int i = 0; i < s; ++i )
        bt->m_I.SetA1( exit_fail[i], exit_point );

    // Exit Debug scope
    bt->m_I.PopDebugScope( bt, m_Chaff, ACT_EXECUTE );
}

void ParallelNodeGrist::GenerateDestructionCode( BehaviourTree* bt )
{
    // Enter Debug scope
    bt->m_I.PushDebugScope( bt, m_Chaff, ACT_DESTRUCT );

    NodeList::const_iterator it( m_Children->begin() ), it_e( m_Children->end() );
    for( ; it != it_e; ++it )
    {
        //call child-node destruction code
        (*it)->m_Grist->GenerateDestructionCode( bt );
    }

    // Exit Debug scope
    bt->m_I.PopDebugScope( bt, m_Chaff, ACT_DESTRUCT );
}

void ParallelNodeGrist::Visit( INodeVisitor* nv )
{
    if( !m_Children )
        return;

    NodeList::iterator it( m_Children->begin() ), it_e( m_Children->end() );
    for( ; it != it_e; ++it )
        (*it)->Visit( nv );
}

void ParallelNodeGrist::SetChaff( Node* n )
{
    NodeGrist::SetChaff( n );
    if( !m_Children )
        return;
    NodeList::iterator it( m_Children->begin() ), it_e( m_Children->end() );
    for( ; it != it_e; ++it )
        (*it)->m_Parent = n;
}

void ParallelNodeGrist::SetChildList( NodeList* children )
{
    m_Children = children;
}

NodeList* ParallelNodeGrist::GetChildList()
{
    return m_Children;
}































