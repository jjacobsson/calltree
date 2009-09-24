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

#include <callback/instructions.h>

using namespace callback;

DecoratorNodeGrist::DecoratorNodeGrist()
    : m_Decorator( 0x0 )
    , m_Child( 0x0 )
    , m_bssPos( 0 )
    , m_bssModPos( 0 )
{
}

DecoratorNodeGrist::~DecoratorNodeGrist()
{
}

void DecoratorNodeGrist::GenerateConstructionCode( BehaviourTree* bt )
{
    int fid = m_Decorator->m_FunctionId;

    // Enter Debug scope
    bt->GetCodeSection().PushDebugScope( bt, m_Chaff, ACT_CONSTRUCT );

    //Store the variable values in the data section.
    StoreVariablesInDataSection( bt, m_Decorator->m_Grist->m_Variables );

    // Load bss section with pointers to the data section, for the variables
    GenerateVariableInstructions( bt );

    int bss_need = m_Decorator->m_Grist->m_bssSize;
    if( m_Decorator->m_Modify )
        bss_need += 4;

    if( bss_need > 0 )
    {
        m_bssPos    = bt->GetBSSSection().Push( bss_need, 4 );
        m_bssModPos = (m_bssPos + bss_need) - 4;
    }

    if( m_Decorator->m_Grist->m_Construct )
    {
        //Setup the register for the data pointer
        SetupVariableRegistry( bt );
        // Load bss register with bss pointer
        bt->GetCodeSection().Push( INST_STORE_PB_IN_R, 1, m_bssPos, 0 );
        // Load the register with the correct id
        bt->GetCodeSection().Push( INST_LOAD_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid & 0x0000ffff );
        // Call the decorator construction function
        bt->GetCodeSection().Push( INST_CALL_CONS_FUN, 0, 1, 2 );
    }

    m_Child->m_Grist->GenerateConstructionCode( bt );

    // Exit Debug scope
    bt->GetCodeSection().PopDebugScope( bt, m_Chaff, ACT_CONSTRUCT );

}

void DecoratorNodeGrist::GenerateExecutionCode( BehaviourTree* bt )
{
    int fid = m_Decorator->m_FunctionId;

    // Enter Debug scope
    bt->GetCodeSection().PushDebugScope( bt, m_Chaff, ACT_EXECUTE );

    int jump_out = -1;

    if( m_Decorator->m_Prune )
    {
        // Enter Debug scope
        bt->GetCodeSection().PushDebugScope( bt, m_Chaff, ACT_PRUNE );

        //Setup the register for the data pointer
        SetupVariableRegistry( bt );
        // Load bss register with bss pointer
        bt->GetCodeSection().Push( INST_STORE_PB_IN_R, 1, m_bssPos, 0 );
        // Load the register with the correct id
        bt->GetCodeSection().Push( INST_LOAD_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid & 0x0000ffff );
        // Call the decorator prune function
        bt->GetCodeSection().Push( INST_CALL_PRUN_FUN, 0, 1, 2 );

        // Exit Debug scope
        bt->GetCodeSection().PopDebugScope( bt, m_Chaff, ACT_PRUNE );

        //Jump out if non-success.
        jump_out = bt->GetCodeSection().Count();
        bt->GetCodeSection().Push( INST_JABC_R_DIFF_C, 0xffffffff, E_NODE_SUCCESS, 0 );
    }

    //Generate child execution code
    m_Child->m_Grist->GenerateExecutionCode( bt );

    if( m_Decorator->m_Modify )
    {
        // Enter Debug scope
        bt->GetCodeSection().PushDebugScope( bt, m_Chaff, ACT_MODIFY );

        //Copy return value to bss section
        bt->GetCodeSection().Push( INST__STORE_R_IN_B, m_bssModPos, 0, 0 );

        //Setup the register for the data pointer
        SetupVariableRegistry( bt );
        // Load bss register with bss pointer
        bt->GetCodeSection().Push( INST_STORE_PB_IN_R, 1, m_bssPos, 0 );
        // Load the register with the correct id
        bt->GetCodeSection().Push( INST_LOAD_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid & 0x0000ffff );
        //Call the decorator modify function
        bt->GetCodeSection().Push( INST_CALL_MODI_FUN, 0, 1, 2 );

        // Exit Debug scope
        bt->GetCodeSection().PopDebugScope( bt, m_Chaff, ACT_MODIFY );
    }

    if( jump_out != -1 )
        bt->GetCodeSection().SetA1( jump_out, bt->GetCodeSection().Count() );

    // Exit Debug scope
    bt->GetCodeSection().PopDebugScope( bt, m_Chaff, ACT_EXECUTE );
}

void DecoratorNodeGrist::GenerateDestructionCode( BehaviourTree* bt )
{
    int fid = m_Decorator->m_FunctionId;

    // Enter Debug scope
    bt->GetCodeSection().PushDebugScope( bt, m_Chaff, ACT_DESTRUCT );

    m_Child->m_Grist->GenerateDestructionCode( bt );

    if( m_Decorator->m_Grist->m_Destruct )
    {
        //Setup the register for the data pointer
        SetupVariableRegistry( bt );
        // Load bss register with bss pointer
        bt->GetCodeSection().Push( INST_STORE_PB_IN_R, 1, m_bssPos, 0 );
        // Load the register with the correct id
        bt->GetCodeSection().Push( INST_LOAD_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid & 0x0000ffff );
        // Call the decorator destruciton function
        bt->GetCodeSection().Push( INST_CALL_DEST_FUN, 0, 1, 2 );
    }

    // Exit Debug scope
    bt->GetCodeSection().PopDebugScope( bt, m_Chaff, ACT_DESTRUCT );
}

void DecoratorNodeGrist::Visit( INodeVisitor* nv )
{
    if( !m_Child )
        return;
    m_Child->Visit( nv );
}

void DecoratorNodeGrist::SetChaff( Node* n )
{
    NodeGrist::SetChaff( n );
    if( !m_Child )
        return;
    m_Child->m_Parent = n;
}

void DecoratorNodeGrist::SetDecorator( Decorator* dec )
{
    m_Decorator = dec;
}

Decorator* DecoratorNodeGrist::GetDecorator()
{
    return m_Decorator;
}

void DecoratorNodeGrist::SetChild( Node* child )
{
    m_Child = child;
}

Node* DecoratorNodeGrist::GetChild()
{
    return m_Child;
}




