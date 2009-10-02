/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "../../code/bt_program.h"
#include <btree/btree.h>

using namespace callback;

SequenceNodeGrist::SequenceNodeGrist()
    : m_Children( 0x0 )
{
}

SequenceNodeGrist::~SequenceNodeGrist()
{
    if( m_Chaff && m_Chaff->m_Tree )
    {
        m_Chaff->m_Tree->FreeNodeList( m_Children );
    }
}

void SequenceNodeGrist::GenerateConstructionCode( BehaviourTree* bt )
{
    // Enter Debug scope
    bt->GetCodeSection().PushDebugScope( bt, m_Chaff, ACT_CONSTRUCT );

    //Alloc storage area for function call stack.
    m_bss_JumpBackTarget    = bt->GetBSSSection().Push( sizeof( int ), 4 );

    //Alloc storage area for re-entry instruction
    m_bss_ReEntry           = bt->GetBSSSection().Push( sizeof( int ), 4 );

    //Set jump-back pointer value to uninitialized
    bt->GetCodeSection().Push( INST__STORE_C_IN_B, m_bss_JumpBackTarget, 0xffffffff, 0 );

    //Set re-entry pointer value to uninitialized
    bt->GetCodeSection().Push( INST__STORE_C_IN_B, m_bss_ReEntry, 0xffffffff, 0 );

    // Exit Debug scope
    bt->GetCodeSection().PopDebugScope( bt, m_Chaff, ACT_CONSTRUCT );
}

void SequenceNodeGrist::GenerateExecutionCode( BehaviourTree* bt )
{
    // Enter Debug scope
    bt->GetCodeSection().PushDebugScope( bt, m_Chaff, ACT_EXECUTE );

    typedef std::vector<int> PatchList;
    PatchList exit_running;
    PatchList exit_fail;
    PatchList dest_jumps;

    //Jump to re-entry point if set
    bt->GetCodeSection().Push( INST_JABB_C_DIFF_B, m_bss_ReEntry, 0xffffffff, m_bss_ReEntry );

    NodeList::const_iterator it( m_Children->begin() ), it_e( m_Children->end() );
    for( ; it != it_e; ++it )
    {
        //call child-node construction code
        (*it)->m_Grist->GenerateConstructionCode( bt );

        //store re-entry pointer
        bt->GetCodeSection().Push( INST__STORE_C_IN_B, m_bss_ReEntry, bt->GetCodeSection().Count() + 1, 0 );

        //call child-node execution code
        (*it)->m_Grist->GenerateExecutionCode( bt );

        //set the destruction jump pointer (we re-use the bss for jump-back-target for this)
        dest_jumps.push_back( bt->GetCodeSection().Count() );
        bt->GetCodeSection().Push( INST__STORE_C_IN_B, m_bss_JumpBackTarget, 0xffffffff, 0 );

        //"exit if running" jump.
        exit_running.push_back( bt->GetCodeSection().Count() );
        bt->GetCodeSection().Push( INST_JABC_R_EQUA_C, 0xffffffff, E_NODE_RUNNING, 0 );

        //Jump to destruction code and set the jump-back at the same time
        dest_jumps.push_back( bt->GetCodeSection().Count() );
        bt->GetCodeSection().Push( INST_JABC_S_C_IN_B, 0xffffffff, m_bss_JumpBackTarget,  bt->GetCodeSection().Count() + 1 );

        //exit this node if return value is non-success
        exit_fail.push_back( bt->GetCodeSection().Count() );
        bt->GetCodeSection().Push( INST_JABC_R_DIFF_C, 0xffffffff, E_NODE_SUCCESS, 0 );
    }

    //Success! Jump past all this destruction stuff
    int jump_to_exit_success = bt->GetCodeSection().Count();
    bt->GetCodeSection().Push( INST_JABC_CONSTANT, 0xffffffff, 0, 0 );

    //Here we generate the destruction code for all child-nodes.
    int i = 0;
    for( it = m_Children->begin(); it != it_e; ++it, ++i )
    {
        //Patch store destruction code pointer instruction
        bt->GetCodeSection().SetA2( dest_jumps[0+i*2], bt->GetCodeSection().Count() );
        //Patch jump to destruction instruction
        bt->GetCodeSection().SetA1( dest_jumps[1+i*2], bt->GetCodeSection().Count() );
        //call child-node destruction code
        (*it)->m_Grist->GenerateDestructionCode( bt );
        //Jump back to calling code and "de-initialize the jump-back-target at the same time
        bt->GetCodeSection().Push( INST_JABB_S_C_IN_B, m_bss_JumpBackTarget, m_bss_JumpBackTarget, 0xffffffff );
    }

    //Set return value to node fail
    int exit_fail_point = bt->GetCodeSection().Count();
    bt->GetCodeSection().Push( INST__STORE_C_IN_R, E_NODE_FAIL, 0, 0 );

    //Patch the "jump to exit success"
    bt->GetCodeSection().SetA1( jump_to_exit_success, bt->GetCodeSection().Count() );

    //clear re-entry pointer
    bt->GetCodeSection().Push( INST__STORE_C_IN_B, m_bss_ReEntry, 0xffffffff, 0 );

    //Patch jump instruction targets for fail.
    int s;
    s = exit_fail.size();
    for( int i = 0; i < s; ++i )
        bt->GetCodeSection().SetA1( exit_fail[i], exit_fail_point );

    //Patch jump instruction targets for running.
    int exit_running_point = bt->GetCodeSection().Count();
    s = exit_running.size();
    for( int i = 0; i < s; ++i )
        bt->GetCodeSection().SetA1( exit_running[i], exit_running_point );

    // Exit Debug scope
    bt->GetCodeSection().PopDebugScope( bt, m_Chaff, ACT_EXECUTE );
}

void SequenceNodeGrist::GenerateDestructionCode( BehaviourTree* bt )
{
    // Enter Debug scope
    bt->GetCodeSection().PushDebugScope( bt, m_Chaff, ACT_DESTRUCT );

    //Jump past destruction code if m_bss_JumpBackTarget is uninitialized
    bt->GetCodeSection().Push( INST_JABC_C_EQUA_B, bt->GetCodeSection().Count() + 2, 0xffffffff, m_bss_JumpBackTarget );
    //Jump to destruction code and set the jump-back at the same time
    bt->GetCodeSection().Push( INST_JABB_S_C_IN_B, m_bss_JumpBackTarget, m_bss_JumpBackTarget, bt->GetCodeSection().Count() + 1 );

    // Exit Debug scope
    bt->GetCodeSection().PopDebugScope( bt, m_Chaff, ACT_DESTRUCT );
}

void SequenceNodeGrist::SetChaff( Node* n )
{
    NodeGrist::SetChaff( n );
    if( !m_Children )
        return;
    NodeList::iterator it( m_Children->begin() ), it_e( m_Children->end() );
    for( ; it != it_e; ++it )
        (*it)->m_Parent = n;
}

void SequenceNodeGrist::SetChildList( NodeList* children )
{
    m_Children = children;
}

NodeList* SequenceNodeGrist::GetChildList()
{
    return m_Children;
}

