/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef VARIABLE_OWNER_HELPER_H_
#define VARIABLE_OWNER_HELPER_H_

#include "node_grist.h"
#include "variable.h"
#include <vector>

class VariableOwner : public NodeGrist
{
public:

    VariableOwner();
    ~VariableOwner();

    void SetVariableList( Variable* variables );
    Variable* GetVariableList();

protected:

    void StoreVariablesInDataSection( BehaviourTree* bt, Variable* declaration );
    void GenerateVariableInstructions( BehaviourTree* bt );
    void SetupVariableRegistry( BehaviourTree* bt );

    Variable*			m_Variables;
    int                 m_bssStart;
    std::vector<int>    m_Data;
};

#endif /* VARIABLE_OWNER_HELPER_H_ */
