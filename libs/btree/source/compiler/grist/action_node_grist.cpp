/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#include <btree/action_node_grist.h>
#include <btree/action.h>
#include <btree/function_grist.h>
#include "../../code/bt_program.h"
#include <btree/btree.h>

using namespace callback;

ActionNodeGrist::ActionNodeGrist()
    : m_Action( 0x0 )
    , m_bssPos( 0 )
{
}

ActionNodeGrist::~ActionNodeGrist()
{
}

void ActionNodeGrist::GenerateConstructionCode( BehaviourTree* bt )
{
    int fid = m_Action->m_FunctionId;

    // Enter Debug scope
    bt->GetCodeSection().PushDebugScope( bt, m_Chaff, ACT_CONSTRUCT );

    //Store the variable values in the data section.
    StoreVariablesInDataSection( bt, m_Action->m_Grist->m_Variables );

    // Load bss section with pointers to the data section, for the variables
    GenerateVariableInstructions( bt );

    if( m_Action->m_Grist->m_bssSize > 0 )
        m_bssPos = bt->GetBSSSection().Push( m_Action->m_Grist->m_bssSize, 4 );

    if( m_Action->m_Grist->m_Construct )
    {
        //Setup the register for the data pointer
        SetupVariableRegistry( bt );
        // Load bss register with bss pointer
        bt->GetCodeSection().Push( INST_STORE_PB_IN_R, 1, m_bssPos, 0 );
        // Load the callback id register with the correct id
        bt->GetCodeSection().Push( INST_LOAD_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid & 0x0000ffff );
        // Call the construction callback
        bt->GetCodeSection().Push( INST_CALL_CONS_FUN, 0, 1, 2 );
    }

    // Exit Debug scope
    bt->GetCodeSection().PopDebugScope( bt, m_Chaff, ACT_CONSTRUCT );
}

void ActionNodeGrist::GenerateExecutionCode( BehaviourTree* bt )
{
    int fid = m_Action->m_FunctionId;
    // Enter Debug scope
    bt->GetCodeSection().PushDebugScope( bt, m_Chaff, ACT_EXECUTE );
    //Setup the register for the data pointer
    SetupVariableRegistry( bt );
    // Load bss register with bss pointer
    bt->GetCodeSection().Push( INST_STORE_PB_IN_R, 1, m_bssPos, 0 );
    // Load the callback id register with the correct id
    bt->GetCodeSection().Push( INST_LOAD_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid & 0x0000ffff );
    // Call the destruction callback
    bt->GetCodeSection().Push( INST_CALL_EXEC_FUN, 0, 1, 2 );
    // Exit Debug scope
    bt->GetCodeSection().PopDebugScope( bt, m_Chaff, ACT_EXECUTE );
}

void ActionNodeGrist::GenerateDestructionCode( BehaviourTree* bt )
{
    int fid = m_Action->m_FunctionId;
    // Enter Debug scope
    bt->GetCodeSection().PushDebugScope( bt, m_Chaff, ACT_DESTRUCT );

    if( m_Action->m_Grist->m_Destruct )
    {
        //Setup the register for the data pointer
        SetupVariableRegistry( bt );
        // Load bss register with bss pointer
        bt->GetCodeSection().Push( INST_STORE_PB_IN_R, 1, m_bssPos, 0 );
        // Load the callback id register with the correct id
        bt->GetCodeSection().Push( INST_LOAD_REGISTRY, 0, (fid >> 16) & 0x0000ffff, fid & 0x0000ffff );
        // Call the destruction callback
        bt->GetCodeSection().Push( INST_CALL_DEST_FUN, 0, 1, 2 );
    }

    // Exit Debug scope
    bt->GetCodeSection().PopDebugScope( bt, m_Chaff, ACT_DESTRUCT );
}


void ActionNodeGrist::SetAction( Action* action )
{
    m_Action = action;
}

Action* ActionNodeGrist::GetAction()
{
    return m_Action;
}





