/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "../bt_program.h"
#include <btree/btree.h>
#include <other/lookup3.h>

using namespace callback;
/*
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
	Variable* t = FindVariableWithIdHash( m_Decorator->m_Vars, hashlittle( "id" ) );
	int fid = t?ValueAsInteger(*t):~0;


    // Enter Debug scope
    bt->GetCodeSection().PushDebugScope( bt, m_Chaff, ACT_CONSTRUCT );

    //Store the variable values in the data section.
    StoreVariablesInDataSection( bt, m_Decorator->m_Args );

    // Load bss section with pointers to the data section, for the variables
    GenerateVariableInstructions( bt );

    t = FindVariableWithIdHash( m_Decorator->m_Vars, hashlittle( "bss" ) );
    int bss_need = t?ValueAsInteger(*t):0;
    t = FindVariableWithIdHash( m_Decorator->m_Vars, hashlittle( "modify" ) );
    if( t && ValueAsBool( *t ) )
        bss_need += 4;

    if( bss_need > 0 )
    {
        m_bssPos    = bt->GetBSSSection().Push( bss_need, 4 );
        m_bssModPos = (m_bssPos + bss_need) - 4;
    }

    t = FindVariableWithIdHash( m_Decorator->m_Vars, hashlittle( "construct" ) );
    if( t && ValueAsBool( *t ) )
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
	Variable* t = FindVariableWithIdHash( m_Decorator->m_Vars, hashlittle( "id" ) );
	int fid = t?ValueAsInteger(*t):~0;


    // Enter Debug scope
    bt->GetCodeSection().PushDebugScope( bt, m_Chaff, ACT_EXECUTE );

    int jump_out = -1;

    t = FindVariableWithIdHash( m_Decorator->m_Vars, hashlittle( "prune" ) );
    if( t && ValueAsBool( *t ) )
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

    t = FindVariableWithIdHash( m_Decorator->m_Vars, hashlittle( "modify" ) );
    if( t && ValueAsBool( *t ) )
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
	Variable* t = FindVariableWithIdHash( m_Decorator->m_Vars, hashlittle( "id" ) );
	int fid = t?ValueAsInteger(*t):~0;

    // Enter Debug scope
    bt->GetCodeSection().PushDebugScope( bt, m_Chaff, ACT_DESTRUCT );

    m_Child->m_Grist->GenerateDestructionCode( bt );

    t = FindVariableWithIdHash( m_Decorator->m_Vars, hashlittle( "destruct" ) );
    if( t && ValueAsBool( *t ) )
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

*/


