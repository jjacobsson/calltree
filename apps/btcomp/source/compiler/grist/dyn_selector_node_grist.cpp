/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "dyn_selector_node_grist.h"
#include "../node.h"
#include "../node_list.h"
#include "../../code/bt_program.h"
#include "../../btree/btree.h"

using namespace callback;

DynamicSelectorNodeGrist::DynamicSelectorNodeGrist( )
    : m_Children( 0x0 )
{
}

DynamicSelectorNodeGrist::~DynamicSelectorNodeGrist()
{
    if( m_Chaff && m_Chaff->m_Tree )
    {
        m_Chaff->m_Tree->FreeNodeList( m_Children );
    }
}

void DynamicSelectorNodeGrist::GenerateConstructionCode( BehaviourTree* bt )
{
    // Enter Debug scope
    bt->m_I.PushDebugScope( bt, m_Chaff, ACT_CONSTRUCT );

    //Alloc storage area in bss
    m_bss_NewBranch             = bt->m_B.Push( sizeof( int ), 4 );
    m_bss_OldBranch             = bt->m_B.Push( sizeof( int ), 4 );
    m_bss_JumpBackTarget        = bt->m_B.Push( sizeof( int ), 4 );
    m_bss_RunningChild          = bt->m_B.Push( sizeof( int ) * m_Children->Size(), 4 );

    int i = 0;
    NodeList::const_iterator it( m_Children->begin() ), it_e( m_Children->end() );
    for( ; it != it_e; ++it, ++i )
    {
        //calculate where in the bss this child's return value should be stored
        int bss_current_child = m_bss_RunningChild + (i * sizeof(int));
        //Set bss return value to undefined for all child nodes.
        bt->m_I.Push( INST__STORE_C_IN_B, bss_current_child, E_NODE_UNDEFINED, 0 );
    }

    //Set bss values to undefined
    bt->m_I.Push( INST__STORE_C_IN_B, m_bss_NewBranch, 0xffffffff, 0 );
    bt->m_I.Push( INST__STORE_C_IN_B, m_bss_OldBranch, 0xffffffff, 0 );

    // Exit Debug scope
    bt->m_I.PopDebugScope( bt, m_Chaff, ACT_CONSTRUCT );
}

void DynamicSelectorNodeGrist::GenerateExecutionCode( BehaviourTree* bt )
{
    // Enter Debug scope
    bt->m_I.PushDebugScope( bt, m_Chaff, ACT_EXECUTE );

    PatchList exit_jumps, true_exit_jumps, cons_jumps, exec_jumps, dest_jumps;
    int i = 0;
    NodeList::const_iterator it( m_Children->begin() ), it_e( m_Children->end() );
    for( ; it != it_e; ++it, ++i )
    {
        //calculate where in the bss this child's return value should be stored
        int bss_current_child = m_bss_RunningChild + (i * sizeof(int));

        //Set jump back after construction target.
        bt->m_I.Push( INST__STORE_C_IN_B, m_bss_JumpBackTarget, bt->m_I.Count() + 2, 0 );
        //Jump to construction code *if* this child is NOT running.
        cons_jumps.push_back( bt->m_I.Count() );
        bt->m_I.Push( INST_JABC_C_DIFF_B, 0xffffffff, E_NODE_RUNNING, bss_current_child );

        //Set this as the "new" executing branch.
        dest_jumps.push_back( bt->m_I.Count() );
        bt->m_I.Push( INST__STORE_C_IN_B, m_bss_NewBranch, 0xffffffff, 0 );

        //Jump to execution code and set the jump-back at the same time
        exec_jumps.push_back( bt->m_I.Count() );
        bt->m_I.Push( INST_JABC_S_C_IN_B, 0xffffffff, m_bss_JumpBackTarget, bt->m_I.Count() + 1 );

        //exit if running
        exit_jumps.push_back( bt->m_I.Count() );
        bt->m_I.Push( INST_JABC_R_EQUA_C, 0xffffffff, E_NODE_RUNNING, 0 );

        //Jump to destruction code and set the jump-back at the same time
        dest_jumps.push_back( bt->m_I.Count() );
        bt->m_I.Push( INST_JABC_S_C_IN_B, 0xffffffff, m_bss_JumpBackTarget,  bt->m_I.Count() + 1 );

        //exit if success
        exit_jumps.push_back( bt->m_I.Count() );
        bt->m_I.Push( INST_JABC_R_EQUA_C, 0xffffffff, E_NODE_SUCCESS, 0 );
    }

    // Truly jump to exit instruction... this happens when all nodes fail
    true_exit_jumps.push_back( bt->m_I.Count() );
    bt->m_I.Push( INST_JABC_CONSTANT, 0xffffffff, 0, 0 );

    i = 0;
    for( it = m_Children->begin(); it != it_e; ++it, ++i )
    {
        //calculate where in the bss this child's return value should be stored
        int bss_current_child = m_bss_RunningChild + (i * sizeof(int));

        //Patch jump to construction instruction
        bt->m_I.SetA1( cons_jumps[i], bt->m_I.Count() );
        //call child-node construction code
        (*it)->m_Grist->GenerateConstructionCode( bt );
        //Jump back to calling code
        bt->m_I.Push( INST_JABB_BSSVALUE, m_bss_JumpBackTarget, 0, 0 );

        //Patch jump to execution instruction
        bt->m_I.SetA1( exec_jumps[i], bt->m_I.Count() );
        //call child-node execution code
        (*it)->m_Grist->GenerateExecutionCode( bt );
        //Store child's return value
        bt->m_I.Push( INST__STORE_R_IN_B, bss_current_child, 0, 0 );
        //Jump back to calling code
        bt->m_I.Push( INST_JABB_BSSVALUE, m_bss_JumpBackTarget, 0, 0 );

        //Patch store destruction code pointer instruction
        bt->m_I.SetA2( dest_jumps[0+i*2], bt->m_I.Count() );
        //Patch jump to destruction instruction
        bt->m_I.SetA1( dest_jumps[1+i*2], bt->m_I.Count() );
        //call child-node destruction code
        (*it)->m_Grist->GenerateDestructionCode( bt );
        //Set childs stored return value to undefined
        bt->m_I.Push( INST__STORE_C_IN_B, bss_current_child, E_NODE_UNDEFINED, 0 );
        //Jump back to calling code
        bt->m_I.Push( INST_JABB_BSSVALUE, m_bss_JumpBackTarget, 0, 0 );
    }

    //Time to patch some jumps! wheee!

    //Patch exit jumps
    int exit_point = bt->m_I.Count();
    int s = exit_jumps.size();
    for( int i = 0; i < s; ++i )
        bt->m_I.SetA1( exit_jumps[i], exit_point );

    //Jump past all this crap if "old branch" is uninitialized
    true_exit_jumps.push_back( bt->m_I.Count() );
    bt->m_I.Push( INST_JABC_C_EQUA_B, 0xffffffff, 0xffffffff, m_bss_OldBranch );
    //set jump back after destruction target
    bt->m_I.Push( INST__STORE_C_IN_B, m_bss_JumpBackTarget, bt->m_I.Count() + 2, 0 );
    //Jump to destruction code of "old branch" *if* it differs from "new branch"
    bt->m_I.Push( INST_JABB_B_DIFF_B, m_bss_OldBranch, m_bss_NewBranch, m_bss_OldBranch );
    //Copy "new branch" into "old branch"
    bt->m_I.Push( INST__STORE_B_IN_B, m_bss_OldBranch, m_bss_NewBranch, 0 );
    //Set new branch to uninitialized
    bt->m_I.Push( INST__STORE_C_IN_B, m_bss_NewBranch, 0xffffffff, 0 );

    //Patch "true" exit jumps
    exit_point = bt->m_I.Count();
    s = true_exit_jumps.size();
    for( int i = 0; i < s; ++i )
        bt->m_I.SetA1( true_exit_jumps[i], exit_point );

    // Exit Debug scope
    bt->m_I.PopDebugScope( bt, m_Chaff, ACT_EXECUTE );
}

void DynamicSelectorNodeGrist::GenerateDestructionCode( BehaviourTree* bt )
{
    // Enter Debug scope
    bt->m_I.PushDebugScope( bt, m_Chaff, ACT_DESTRUCT );

    //Jump past all this crap if "old branch" is uninitialized
    int patch_exit = bt->m_I.Count();
    bt->m_I.Push( INST_JABC_C_EQUA_B, 0xffffffff, 0xffffffff, m_bss_OldBranch );
    //set jump back after destruction target
    bt->m_I.Push( INST__STORE_C_IN_B, m_bss_JumpBackTarget, bt->m_I.Count() + 2, 0 );
    //Jump to destruction code of "old branch" if it is set to a valid value
    bt->m_I.Push( INST_JABB_C_DIFF_B, m_bss_OldBranch, 0xffffffff, m_bss_OldBranch );
    //Patch the jump instruction
    bt->m_I.SetA1( patch_exit, bt->m_I.Count() );

    // Exit Debug scope
    bt->m_I.PopDebugScope( bt, m_Chaff, ACT_DESTRUCT );
}

void DynamicSelectorNodeGrist::Visit( INodeVisitor* nv )
{
    NodeList::iterator it( m_Children->begin() ), it_e( m_Children->end() );
    for( ; it != it_e; ++it )
        (*it)->Visit( nv );
}

void DynamicSelectorNodeGrist::SetChaff( Node* n )
{
    NodeGrist::SetChaff( n );
    if( !m_Children )
        return;
    NodeList::iterator it( m_Children->begin() ), it_e( m_Children->end() );
    for( ; it != it_e; ++it )
        (*it)->m_Parent = n;
}

void DynamicSelectorNodeGrist::SetChildList( NodeList* children )
{
    m_Children = children;
}

NodeList* DynamicSelectorNodeGrist::GetChildList()
{
    return m_Children;
}







